#include <transport/server.h>
#include <transport/io_ctx.h>
#include "linux_conn.h"
#include "epoll_ctx.h"

#include <stdlib.h>
#include <stdexcept>
#include <string_view>
#include <algorithm>
    

namespace
{
    constexpr int max_epoll_ready_events = 64;

    inline bool would_block(transport::conn_ctx *const conn)
    {
        int err = conn->skt()->get_last_error();
        return err == EAGAIN || err == EWOULDBLOCK;
    }
}

namespace transport
{

    server::server(conn_cb_t on_conn, io_cb_t on_rx, io_cb_t on_tx, io_cb_t on_client_close,
                   io_cb_t on_timeout, const config &config)
        : on_conn_(on_conn), on_rx(on_rx), on_tx(on_tx), on_client_close(on_client_close),
          on_timeout_(on_timeout), config_(config)
    {
        socket::startup();
        spdlog::set_pattern("*** [%H:%M:%S %z] [thread %t] %v ***");

        epoll_ctx_ = std::make_unique<epoll_ctx>(config_.max_concurrent_connections);
        io_workers = std::make_unique<threadpool::pool>(config_.num_req_handler_threads, config_.processing_timeout_sec);
        logger_ = spdlog::stdout_color_mt("transport");
    }

    void server::register_socket(const socket_t _, std::shared_ptr<conn_ctx> conn)
    {
        epoll_ctx_->add_event(EPOLLIN, conn->skt_handle());
    }
    
    std::shared_ptr<conn_ctx> server::add_conn(socket_t skt_handle)
    {
        std::shared_ptr<linux_conn> conn;
        {
            std::unique_lock<std::mutex> lock(conn_mutex_);
            conn = std::make_shared<linux_conn>(skt_handle, epoll_ctx_);
            conns_[skt_handle] = conn;
            logger_->info("[skt {0}] connection added: {1}", skt_handle, conns_.size());
        }
        return conn;
    }

    void server::handle_in(std::shared_ptr<conn_ctx> conn)
    {
        int nbytes = 0;
        bool peer_has_closed = false;
        std::string rx_string;
        std::vector<std::unique_ptr<io_ctx>> rx_ios;

        while (true) {
            std::unique_ptr<io_ctx> rx_io = std::make_unique<io_ctx>(io::type::rx);
            while (rx_io->bytes_rx < rx_io->buf_len_) {
                nbytes = conn->skt()->rx(rx_io.get(), 1);

                if (nbytes == socket_error) {
                    if (would_block(conn.get())) {
                        rx_ios.push_back(std::move(rx_io));
                        goto done;
                    }
                    logger_->error("[skt {0}] rx socket error {1}", conn->skt_handle(), conn->skt()->get_last_error());
                    goto close;
                }
                peer_has_closed = nbytes == 0;
                if (peer_has_closed) {
                    logger_->info("[skt {0}] peer graceful shutdown", conn->skt_handle());
                    rx_ios.push_back(std::move(rx_io));
                    goto done;
                }
                rx_io->bytes_rx += nbytes;
            }
            rx_ios.push_back(std::move(rx_io));
        }

    done:
        for (const auto& io : rx_ios) {
            rx_string.append(io->buf, io->bytes_rx);
        }
        conn->toggle_status(conn_keep_alive, false);
        if (!rx_string.empty()) {
            logger_->info("[skt {0}] rx {1:d} bytes", conn->skt_handle(), rx_string.size());
            conn->on_rx(rx_string);
            on_rx(conn->skt_handle());
        }
        if (!peer_has_closed) {
            return;
        }

    close:
        epoll_ctx_->remove_event(conn->skt_handle());
        remove_conn(conn->skt_handle());
    }

    void server::handle_out(std::shared_ptr<conn_ctx> conn)
    {
        /* TODO: store io_ctx as a field in conn_ctx. Avoids having more key value stores. */
        io_ctx *const tx_io = epoll_ctx_->get_io(conn->skt_handle());
        std::string req_str;
        int nbytes = 0;
        while (tx_io->bytes_tx < tx_io->bytes_to_tx) {
            nbytes = conn->skt()->tx(tx_io, 1); 
            if (nbytes == socket_error) {
                if (would_block(conn.get())) {
                    goto partial;
                }
                logger_->error("[skt {0}] tx socket error {1}", conn->skt_handle(), conn->skt()->get_last_error());
                goto close;
            }
            tx_io->bytes_tx += nbytes;
        }

        logger_->info("[skt {0}] tx completed {1} bytes", conn->skt_handle(), tx_io->bytes_to_tx);
        on_tx(conn->skt_handle());
        if (!conn->keep_alive()) {
            logger_->info("[skt {0}] tx closing", conn->skt_handle(), tx_io->bytes_to_tx);
            goto close;
        }
        return;

    partial:
        logger_->info("[skt {0}] tx partial {1} bytes", conn->skt_handle(), tx_io->bytes_tx);
        req_str = {tx_io->buf + tx_io->bytes_tx, tx_io->bytes_to_tx - tx_io->bytes_tx};
        conn->request_tx(req_str);
        return;
    
    close:    
        epoll_ctx_->remove_event(conn->skt_handle());
        remove_conn(conn->skt_handle());
    }

    void server::run_event_loop(void)
    {
        std::array<epoll_event, max_epoll_ready_events> ready_events;
        while (true) {
            int nready = epoll_ctx_->wait(ready_events.data());
            for (int i = 0; i < std::min(nready, max_epoll_ready_events); i++) {
                epoll_event event = ready_events[i];

                if (event.events & EPOLLERR) {
                    logger_->error("[epollfd {0} | skt {1}] EPOLL error {2}", epoll_ctx_->get_handle(), (int)event.data.fd, errno);
                    continue;
                } 

                std::shared_ptr<conn_ctx> conn;
                {
                    std::unique_lock<std::mutex> lock(conn_mutex_);
                    conn = conns_[event.data.fd];
                }

                if (conn == nullptr) {
                    logger_->error("[skt {0}] EPOLL event ready on closed skt.", (int)event.data.fd);
                }
                else if (event.events & EPOLLIN) {
                    handle_in(conn);
                }
                else if (event.events & EPOLLOUT) {
                    handle_out(conn);
                }
                else {
                    logger_->error("[skt {0}] EPOLL ERROR {1}", conn->skt_handle(), errno);
                    epoll_ctx_->remove_event(conn->skt_handle());
                    remove_conn(conn->skt_handle()); 
                }
            }
        }
    }
}

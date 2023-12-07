#include <transport/server.h>
#include <transport/io_ctx.h>

#include <stdlib.h>
#include <string_view>
    

namespace transport
{

    constexpr int block_indefinitely = -1;


    server::server(conn_cb_t on_conn, io_cb_t on_rx, io_cb_t on_tx, io_cb_t on_client_close,
                   io_cb_t on_timeout, const config &config)
        : on_conn_(on_conn), on_rx(on_rx), on_tx(on_tx), on_client_close(on_client_close),
          on_timeout_(on_timeout), config_(config)
    {
        transport::socket::startup();
        logger_ = spdlog::stdout_color_mt("transport");

        io_workers = std::make_unique<threadpool::pool>(config_.num_req_handler_threads, config_.req_timeout_ms);
        
        queue_handle_ = epoll_create1(EPOLL_CLOEXEC); 
        if (queue_handle_ == invalid_handle) {
            throw std::runtime_error("Failed to create epoll instance.");
        }

        ready_events_ = static_cast<epoll_event*>(calloc(config_.max_concurrent_connections), sizeof(epoll_event)));
    }

    void server::register_socket(const socket_t handle, conn_ctx *const conn)
    {
        if (events_.size() >= config_.max_concurrent_connections) {
            return;
        }

        std::unique_ptr<epoll_event> event = std::make_unique<epoll_event>();
        event->events = EPOLLRDHUP | EPOLLET | EPOLLONESHOT;
        event->data.ptr = conn;

        int err = epoll_ctl(queue_handle_, EPOLL_CTL_ADD, handle, event.get());
        if (err == epoll_error) {
            throw std::runtime_error("Failed to register a socket to this epoll instance.");
        }
        
        std::unique_lock<std::mutex> lock(events_mutex_);
        events_[conn->skt->handle()] = std::move(event);
    }

    /* Per thread event readyness loop. */
    void server::run_event_loop(void)
    {
        while (true) {
            int nready = epoll_wait(queue_handle_, ready_events_, max_epoll_events, block_indefinitely);
            for (int i = 0; i < nready; i++) {
                epoll_event event = events[i];

                if (event.events & EPOLLERR) {
                    return;     
                } 

                conn_ctx* conn = static_cast<conn_ctx*>(event.data.ptr);

                if (event.events & EPOLLHUP) {
                    { 
                        std::unique_lock<std::mutex> lock(conn_mutex_);
                        conns_.erase(conn->skt->handle());
                    }
                    on_client_close(conn->skt->handle());
                }
                else if (event.events & EPOLLIN) {
                    std::unique_ptr<io_ctx> rx_io = std::make_unique<io_ctx>(io::type::rx);
                    conn->skt->rx(rx_io.get(), 1);
                    conn->on_rx(std::string_view(rx_io->buf, rx_io->bytes_rx));

                    try {
                        on_rx(conn->skt->handle());
                    }
                    catch ()
                }
                else if (event.events & EPOLLOUT) {
                    io_ctx *const tx_io = outgoing_io_[conn->skt->handle()].get();
                    conn->skt->tx(tx_io, 1); 

                    if (tx_io->bytes_tx < tx_io->bytes_to_tx) {
                        conn->do_tx(std::string_view(tx_io->buf + tx_io->bytes_tx, tx_io->bytes_to_tx - tx_io->bytes_tx));
                    }
                    else {
                        on_tx(conn->skt->handle());
                    }
                }
            }
        }
    }
    
}
#include <transport/server.h>
#include <transport/io_ctx.h>

#include <stdlib.h>

namespace transport
{

    constexpr int max_epoll_events = 1000;
    constexpr int block_indefinitely = -1;

    server::server(io_cb_t on_rx, io_cb_t on_tx, io_cb_t on_client_close, const config &config)
        : on_rx(on_rx), on_tx(on_tx), on_client_close(on_client_close), config_(config)
    {
        transport::socket::startup();
        logger_ = spdlog::stdout_color_mt("transport");

        io_workers = std::make_unique<threadpool::pool>(config_.num_req_handler_threads, config_.req_timeout_ms);
        
        queue_handle_ = epoll_create1(EPOLL_CLOEXEC); 
        if (queue_handle_ == invalid_handle) {
            throw std::runtime_error("Failed to create epoll instance.");
        }
    }

    void server::register_socket(const socket_t handle, const conn_ctx *const conn)
    {
        if (events_.size() >= max_epoll_events) {
            return;
        }

        std::unique_ptr<epoll_event> event = std::make_unique<epoll_event>(
            EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET | EPOLLONESHOT, conn);

        int err = epoll_ctl(queue_handle_, EPOLL_CTL_ADD, handle, event.get());
        if (err == epoll_error) {
            throw std::runtime_error("Failed to register a socket to this epoll instance.");
        }
        events_.push_back(std::move(event));
    }

    void server::dequeue(void)
    {
        while (true) {
            epoll_event *events = calloc(max_epoll_events, sizeof(epoll_event));
            int nready = epoll_wait(queue_handle_, events_, max_epoll_events, block_indefinitely);
            for (int i = 0; i < nready; i++) {
                epoll_event event = events_[i];

                if (event.events & EPOLLERR) {
                    
                } 
                else if (event.events & EPOLLHUP) {
                    conn_ctx* conn = static_cast<conn_ctx*>(event.data.ptr);
                    { 
                        std::unique_lock<std::mutex> lock(conn_mutex_);
                        conns_.erase(conn->skt->handle());
                    }
                    on_client_close(conn->skt->handle());
                }
                else if (event.events & EPOLLIN) {
                   // perform the read on socket fd.
                    io_ctx *rx_io = new io_ctx(io::type::rx);
                    conn->skt->rx(rx_io, 1);

                    conn->on_rx(std::string_view(rx_io->buf, rx_io->bytes_rx));
                    on_rx(conn->skt->handle());
                }
                else if (event.events & EPOLLOUT) {
                    // perform the write on socket fd.
                }
            }
        }
    }
    
    void server::rx(socket *const skt)
    {
        /* TODO: complete */
        // since we are using EPOLLONESHOT, we want to rearm the event.
        int err = epoll_ctl(queue_handle_, EPOLL_CTL_MOD, skt->handle(), events_[skt->handle()].get());
    }

    void server::tx(socket *const skt, std::string_view msg)
    {
        io_ctx *tx_io = new io_ctx(io::tx);
        tx_io->write_buf(msg);
        /* TODO: complete */
    }
}
#include <transport/server.h>
#include <transport/io_ctx.h>

namespace transport
{
    /* Used as the thread limit when creating a completion port. A value
    of 0 is used to indicate: as many threads as this machine has processors. */
    constexpr int nproc = 0;

    server::server(conn_cb_t on_conn, io_cb_t on_rx, io_cb_t on_tx, io_cb_t on_client_close, const config &config)
        : on_conn_(on_conn), on_rx(on_rx), on_tx(on_tx), on_client_close(on_client_close), config_(config)
    {
        socket::startup();
        logger_ = spdlog::stdout_color_mt("transport");

        io_workers = std::make_unique<threadpool::pool>(config_.num_req_handler_threads, config_.req_timeout_ms);
        
        queue_handle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, nproc);
        if (queue_handle_ == nullptr) {
            throw std::runtime_error("Failed to create io queue.");
        }
    }


    void server::register_socket(const socket_t handle, const conn_ctx *const conn)
    {
        queue_handle_ = CreateIoCompletionPort((HANDLE)handle, queue_handle_, (DWORD_PTR)conn, 0);
        if (queue_handle_ == nullptr) {
            throw std::runtime_error("Failed to add socket to completion port.");
        }
    }

    void server::dequeue(void)
    {
        unsigned long io_size = 0;
        conn_ctx *conn = nullptr;
        io_ctx *io = nullptr;

        while (true) {
            /* attempt to dequeue an IO completion packet from the completion port. */
            bool success = GetQueuedCompletionStatus(queue_handle_, &io_size, (PULONG_PTR)&conn, (LPOVERLAPPED *)&io, INFINITE);
            if (!success) {
                throw std::runtime_error("Failed to dequeue from io queue.");
            }

            if (conn == nullptr) {
                return;
            }

            if (io_size == 0) {
                { 
                    std::unique_lock<std::mutex> lock(conn_mutex_);
                    conns_.erase(conn->skt->handle());
                }
                on_client_close(conn->skt->handle());
                continue;
            }

            std::unique_ptr<io_ctx> io_ctx;
            io_ctx.reset(io);

            if (io->type == io::type::rx) {
                io->bytes_rx = io_size;
                conn->on_rx(std::string_view(io->buf, io->bytes_rx));
                on_rx(conn->skt->handle()); /* TODO: maybe this should just be in conn->on_rx, that way we can customise io handling per connection, instead of per server. */
            }
            else if (io->type == io::type::tx) {
                io->bytes_tx += io_size;

                if (io->bytes_tx < io->bytes_to_tx) {
                    /* not all bytes tx'ed, resubmit. */
                    conn->do_tx(std::string_view(io->buf + io->bytes_tx, io->buf_desc.len - io->bytes_tx));
                }
                else {
                    on_tx(conn->skt->handle());
                }
            }
        }
    }

    void server::rx(socket *const skt)
    {
        io_ctx *const rx_io = new io_ctx(io::type::rx);
        skt->rx(rx_io, 1);
    }

    void server::tx(socket *const skt, std::string_view msg)
    {
        io_ctx *const tx_io = new io_ctx(io::type::tx);
        tx_io->write_buf(msg);
        skt->tx(tx_io, 1);
    }
}

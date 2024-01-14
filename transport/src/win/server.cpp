#include <transport/server.h>
#include <transport/io_ctx.h>
#include "win_conn.h"


namespace beemo
{
    /* Used as the thread limit when creating a completion port. A value
    of 0 is used to indicate: as many threads as this machine has processors. */
    constexpr int nproc = 0;

    eventloop::eventloop(conn_cb_t on_conn, io_cb_t on_rx, io_cb_t on_tx, io_cb_t on_client_close,
                   io_cb_t on_timeout, const config &config)
        : on_conn_(on_conn), on_rx(on_rx), on_tx(on_tx), on_client_close(on_client_close),
          on_timeout_(on_timeout), config_(config)
    {
        socket::startup();
        logger_ = spdlog::stdout_color_mt("transport");

        io_workers = std::make_unique<threadpool::pool>(config_.num_req_handler_threads, config_.processing_timeout_sec);
        
        queue_handle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, nproc);
        if (queue_handle_ == nullptr) {
            throw transport_err();
        }
    }

    std::shared_ptr<conn_ctx> eventloop::add_conn(socket_t skt_handle)
    {
        std::shared_ptr<win_conn> conn = std::make_shared<win_conn>(skt_handle);
        logger_->info("Connections: {0:d}", conns_.size());
        std::unique_lock<std::mutex> lock(conn_mutex_);
        conns_[conn->skt_handle()] = conn;
        return conn;
    }

    void eventloop::register_socket(const socket_t handle, conn_ctx *const conn)
    {
        io_queue_t res = CreateIoCompletionPort((HANDLE)handle, queue_handle_, (DWORD_PTR)conn, 0);
        if (res == nullptr) {
            throw transport_err();
        }
        queue_handle_ = res;
    }

    /* Per thread event completion loop. */
    void eventloop::run_event_loop(void)
    {
        while (true) {
            unsigned long io_size = 0;
            conn_ctx *conn = nullptr;
            io_ctx *io = nullptr;
            
            bool success = GetQueuedCompletionStatus(queue_handle_, &io_size, (PULONG_PTR)&conn, (LPOVERLAPPED *)&io, INFINITE);
            if (!success) {
                logger_->error("Failed to dequeue from completion queue.");
                continue;
            }

            if (conn == nullptr) {
                logger_->error("IO operaton has no associated connection.");
                continue;
            }

            if (io_size == 0) {
                remove_conn(conn->skt_handle());
                continue;
            }

            std::unique_ptr<io_ctx> io_ctx;
            io_ctx.reset(io);

            if (io->type == io::type::rx) {
                io->bytes_rx = io_size;
                conn->on_rx(std::string_view(io->buf, io->bytes_rx));

                cb_with_timeout(on_rx, conn->skt_handle());
            }
            else if (io->type == io::type::tx) {
                io->bytes_tx += io_size;

                if (io->bytes_tx < io->bytes_to_tx) {
                    /* not all bytes tx'ed, resubmit. */
                    conn->request_tx(std::string_view(io->buf + io->bytes_tx, io->buf_desc.len - io->bytes_tx));
                }
                else {
                    on_tx(conn->skt_handle());
                }
            }
        }
    } 
    
}

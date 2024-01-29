#include <ev/evloop.h>
#include <transport/platform.h>

namespace beemo {

    struct __evloop_ctx {
        io_queue_t iocp_handle_;
        __evloop_ctx(io_queue_t iocp_handle) : iocp_handle_(iocp_handle) { }
    };

    evloop::evloop(uint max_evs)
    {
        socket::startup();
        logger_ = spdlog::stdout_color_mt("transport");
        ctx_ = new __evloop_ctx(epoll_create(max_evs_));
        ctx_->iocp_handle_ = CreateIoCompletionPort(invalid_handle, nullptr, 0, 0);
        if (ctx_->iocp_handle_ == nullptr) {
            throw transport_err();
        }
    }

    evloop::~evloop()
    {
        delete[] ctx_;
    }

    int evloop::reg(std::shared_ptr<conn> conn, ev_mask mask) {
        /* TODO: this should be done once at connection. */
        socket_t sktfd = conn->skt_->handle_;
        io_queue_t res = CreateIoCompletionPort(sktfd, ctx_->iocp_handle_, sktfd, 0);
        if (res == nullptr) {
            logger_->info("Failed to register {0}, errno: {1}", sktfd, errno);
            return ev_error;
        }
        ctx_->iocp_handle_ = res;

        switch (mask) {
            case EV_IN:
                conn->do_rx();
                break;
            case EV_OUT:
                conn->do_tx();
                break;
            default:
                return ev_error;
        }
        
        std::unique_lock<std::mutex> lock(events_mtx_);
        events_[sktfd] = conn;
        return 0;
    }

    evloop::loop(uint max_evs, int timeout)
    {
        for (;;) { 
            unsigned long io_size = 0;
            io_buf *io = nullptr;
            int sktfd = socket_error;
            
            bool success = GetQueuedCompletionStatus(ctx_->iocp_handle_, &io_size, (PULONG_PTR)&sktfd, (LPOVERLAPPED *)&io, INFINITE);
            if (!success) {
                logger_->error("Failed to dequeue from completion queue.");
                continue;
            }

            std::weak_ptr<conn> ev_conn = get_event(sktfd);
            if (&ev_conn == nullptr || ev_conn.expired()) {
                logger_->error("IO operaton has no associated connection.");
                continue;
            }
            std::shared_ptr<conn> conn = ev_conn.lock();

            if (io_size == 0) {
                conn->on_close_(conn);
                continue;
            }

            std::unique_ptr<io_buf> io_buf;
            io_buf.reset(io);

            if (io_buf->type == io_type::RX) {
                io_buf->bytes_rx = io_size;
                conn->on_rx_(std::string_view(io_buf->buf, io_buf->bytes_rx));
            }
            else if (io_buf->type == io_type::TX) {
                io_buf->bytes_tx += io_size;
                if (io_buf->bytes_tx < io_buf->bytes_to_tx) {
                    /* not all bytes tx'ed, resubmit. */
                    conn->prepare_tx(std::string_view(io_buf->buf + io_buf->bytes_tx, io_buf->buf_desc.len - io_buf->bytes_tx));
                    conn->do_tx();
                }
                else {
                    conn->on_tx_(conn);
                }
            }
        }
    }
}
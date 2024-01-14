#include <ev/evloop.h>
#include <sys/epoll.h>

#include <transport/io_buf.h>
#include <transport/socket.h>

#include <map>
#include <exception>


namespace beemo
{
    constexpr int default_mask = EPOLLHUP | EPOLLET | EPOLLONESHOT;

    /* One per evloop. */
    struct __evloop_ctx {
        int epfd_;
        __evloop_ctx(int epfd) : epfd_(epfd) { }
    };

    evloop::evloop(uint max_evs) : max_evs_(max_evs)
    {
        logger_ = spdlog::stdout_color_mt("ev_epoll");
        ctx_ = new __evloop_ctx(epoll_create(max_evs_));
        if (ctx_->epfd_ == ev_error) {
            throw std::exception();
        }
    }

    evloop::~evloop()
    {
        delete[] ctx_;
    }

    /* Register a connection with the event loop. */
    int evloop::reg(std::shared_ptr<conn> c, ev_mask mask)
    {
        epoll_event event {0}; 
        socket_t sktfd = c->skt_->handle_;

        int op = EPOLL_CTL_MOD;
        if (get_event(sktfd).expired()) {
            op = EPOLL_CTL_ADD;    
        }

        event.data.fd = sktfd;
        switch (mask) {
            case EV_IN: 
                event.events = default_mask | EPOLLIN; 
                break;
            case EV_OUT:
                event.events = default_mask | EPOLLOUT; 
                break;
            default:
                return ev_error;
        }

        if (epoll_ctl(ctx_->epfd_, op, sktfd, &event) == ev_error) {
            logger_->info("Failed to register {0}, errno: {1}", sktfd, errno);
            return ev_error;
        }

        std::unique_lock<std::mutex> lock(events_mtx_);
        events_[sktfd] = c;
        return 0;
    }

    void evloop::loop(uint max_events, int timeout)
    {
        epoll_event* events = new epoll_event[max_events];
        for(;;) {
            int nready = epoll_wait(ctx_->epfd_, events, max_events, timeout);
            for (int i = 0; i < nready; i++) {
                int mask = events[i].events;
                int sktfd = events[i].data.fd;

                if (mask & EPOLLERR) {
                    logger_->error("[epollfd {0} | skt {1}] EPOLL error {2}", ctx_->epfd_, sktfd, errno);
                    continue;
                } 

                std::weak_ptr<conn> ev_conn = get_event(sktfd);
                if (&ev_conn == nullptr || ev_conn.expired()) {
                    logger_->error("[skt {0}] EPOLL event ready on closed skt.", sktfd);
                    continue;
                }

                std::shared_ptr<conn> conn = ev_conn.lock();
                if (mask & EPOLLIN) {
                    handle_in(conn);
                }
                else if (mask & EPOLLOUT) {
                    handle_out(conn);
                }
                else {
                    logger_->error("[skt {0}] EPOLL ERROR {1}", sktfd, errno);
                }
            }
        }
    }
    
    void evloop::handle_in(std::shared_ptr<conn> conn)
    {
        int nbytes = 0;
        bool peer_has_closed = false;
        std::string rx_string; /* TODO: can we make this a string_view. */
        std::vector<std::unique_ptr<io_buf>> rx_ios;

        while (true) {
            std::unique_ptr<io_buf> rx_io = std::make_unique<io_buf>(RX);
            while (rx_io->bytes_rx < rx_io->buf_len_) {
                nbytes = conn->skt_->rx(rx_io.get(), 1);

                if (nbytes == socket_error) {
                    if (conn->skt_->would_block()) {
                        rx_ios.push_back(std::move(rx_io));
                        goto done;
                    }
                    logger_->error("[skt {0}] rx socket error {1}", conn->skt_->handle_, conn->skt_->last_error_);
                    goto close;
                }
                peer_has_closed = nbytes == 0;
                if (peer_has_closed) {
                    logger_->info("[skt {0}] peer graceful shutdown", conn->skt_->handle_);
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
        if (!rx_string.empty()) {
            logger_->info("[skt {0}] rx {1:d} bytes", conn->skt_->handle_, rx_string.size());
            conn->io_rx_ = rx_string;
            conn->on_rx_(conn->skt_->handle_);
        }
        return;

        if (!peer_has_closed) {
            return;
        }
        logger_->info("[skt {0}] rx closing gracefully", conn->skt_->handle_);

    close:
        conn->on_close_(conn->skt_->handle_);
        logger_->info("[skt {0}] rx closing", conn->skt_->handle_);
    }

    void evloop::handle_out(std::shared_ptr<conn> conn)
    {
        std::string req_str;
        int nbytes = 0;
        io_buf* tx_io = conn->io_tx_.get();

        while (tx_io->bytes_tx < tx_io->bytes_to_tx) {
            nbytes = conn->skt_->tx(tx_io, 1); 
            if (nbytes == socket_error) {
                if (conn->skt_->would_block()) {
                    goto partial;
                }
                logger_->error("[skt {0}] tx socket error {1}", conn->skt_->handle_, conn->skt_->last_error_);
                goto close;
            }
            tx_io->bytes_tx += nbytes;
        }

        logger_->info("[skt {0}] tx completed {1} bytes", conn->skt_->handle_, tx_io->bytes_to_tx);
        conn->on_tx_(conn->skt_->handle_);
        return;

    partial:
        logger_->info("[skt {0}] tx partial {1} bytes", conn->skt_->handle_, tx_io->bytes_tx);
        req_str = {tx_io->buf + tx_io->bytes_tx, tx_io->bytes_to_tx - tx_io->bytes_tx};
        conn->prepare_tx(req_str);
        reg(conn, EV_OUT);
        return;
    
    close:
        conn->on_close_(conn->skt_->handle_);
    }
}
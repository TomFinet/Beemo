#include <transport/conn_ctx.h>
#include <transport/platform.h>


namespace transport
{

    void conn_ctx::rx(conn_ctx *const conn)
    {
        epoll_event* rx_event = events_[skt->handle()].get();
        rx_event->events &= ~EPOLLOUT;
        rx_event->events |= EPOLLIN;

        int err = epoll_ctl(queue_handle_, EPOLL_CTL_MOD, conn->skt->handle(), rx_event);
    }

    void conn_ctx::tx(conn_ctx *const conn, std::string_view msg)
    {
        epoll_event* tx_event = events_[skt->handle()].get();
        tx_event->events |= EPOLLOUT;
        tx_event->events &= ~EPOLLIN;
        
        std::unique_ptr<io_ctx> tx_io = std::make_unique<io_ctx>(io::type::tx);
        tx_io->write_buf(msg);
        {
            std::unique_lock<std::mutex> lock(outgoing_mutex_);
            outgoing_io_[skt->handle()] = std::move(tx_io);
        }

        int err = epoll_ctl(queue_handle_, EPOLL_CTL_MOD, conn->skt->handle(), tx_event);
    }

}
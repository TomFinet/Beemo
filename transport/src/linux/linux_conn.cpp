#include "linux_conn.h"
#include "epoll_ctx.h"


namespace transport
{
    void linux_conn::__request_rx(void)
    {
        epoll_ctx_->modify_event(skt_handle(), EPOLLIN);
    }

    void linux_conn::__request_tx(std::string_view msg)
    {
        epoll_ctx_->add_io(skt_handle(), msg);
        epoll_ctx_->modify_event(skt_handle(), EPOLLOUT);
    }
}
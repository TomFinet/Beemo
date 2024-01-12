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
        tx_io_ = std::make_unique<io_ctx>(io::type::tx, msg.size());
        tx_io_->write_buf(msg);
        epoll_ctx_->modify_event(skt_handle(), EPOLLOUT);
    }
}
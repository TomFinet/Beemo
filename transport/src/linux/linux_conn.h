#pragma once

#include <transport/socket.h>
#include <transport/conn_ctx.h>
#include "epoll_ctx.h"

#include <memory>


namespace transport
{
    class linux_conn : public conn_ctx {
        private:
            std::shared_ptr<epoll_ctx> epoll_ctx_;
            std::unique_ptr<io_ctx> tx_io_;

            void __request_rx(void) override;
            void __request_tx(std::string_view msg) override;

        public:
            linux_conn(socket_t skt_handle, std::shared_ptr<epoll_ctx> epoll_ctx)
                : conn_ctx(skt_handle), epoll_ctx_(epoll_ctx) { }

            io_ctx* tx_io(void) override
            {
                return tx_io_.get();
            }
    };
}

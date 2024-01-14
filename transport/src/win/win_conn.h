#pragma once

#include <transport/conn_ctx.h>


namespace beemo
{
    class win_conn : public conn_ctx {
        public:
            void __request_rx(void) override;
            void __request_tx(std::string_view msg) override;
    };
}
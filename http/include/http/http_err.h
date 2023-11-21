#pragma once

#include <string>
#include <memory>

#include <http/conn_ctx.h>

namespace
{
    constexpr size_t max_reason_len = 30;
}

namespace http
{

    class err_handler {
        public:
            virtual void handle(std::shared_ptr<conn_ctx> conn) { } 
    };

    class err_conn_close_handler : public err_handler {
        public:
            void handle(std::shared_ptr<conn_ctx> conn);
    };
    
    class err_response_handler : public err_handler {
        private:
            char reason[max_reason_len];
            unsigned short status_code;
        public:
            err_response_handler(const std::string &reason, unsigned short status_code);
            void handle(std::shared_ptr<conn_ctx> conn);
    };

    /* to be used as singletons globally, hence the external linkage. */ 
    extern err_conn_close_handler close_handler;
    extern err_response_handler bad_req_handler;
    extern err_response_handler not_impl_handler;
    extern err_response_handler len_required_handler;
}
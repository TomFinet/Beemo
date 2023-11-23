#pragma once

#include <string>
#include <memory>

#include <http/conn_ctx.h>

#include <spdlog/spdlog.h>


namespace http
{

    class err_handler {
        public:
            virtual void handle(std::shared_ptr<conn_ctx> conn, std::shared_ptr<spdlog::logger> logger) { } 
    };

    class err_response_handler : public err_handler {
        private:
            std::string reason;
            unsigned short status_code;
        public:
            err_response_handler(const std::string &reason, unsigned short status_code) : reason(reason), status_code(status_code) { }
            void handle(std::shared_ptr<conn_ctx> conn, std::shared_ptr<spdlog::logger> logger) override;
    };

    /* to be used as singletons globally, hence the external linkage. */ 
    extern err_response_handler bad_req_handler;
    extern err_response_handler len_required_handler;
    extern err_response_handler uri_too_long_handler;
    extern err_response_handler unsupported_media_type_handler;
    extern err_response_handler misdirected_req_handler;

    extern err_response_handler not_impl_handler;
}
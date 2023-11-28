#pragma once

#include <http/connection.h>

#include <spdlog/spdlog.h>

#include <string>
#include <memory>


namespace http
{

    class err_handler {
        public:
            /* Don't really want to pass the logger in each time. Maybe make a logger for the errors. */
            virtual void handle(std::shared_ptr<connection> conn, std::shared_ptr<spdlog::logger> logger) { } 
    };

    class err_response_handler : public err_handler {
        private:
            std::string reason;
            unsigned short status_code;
        public:
            err_response_handler(const std::string &reason, unsigned short status_code) : reason(reason), status_code(status_code) { }
            void handle(std::shared_ptr<connection> conn, std::shared_ptr<spdlog::logger> logger) override;
    };

    /* to be used as singletons globally, hence the external linkage. */ 
    extern err_response_handler bad_req_handler;
    extern err_response_handler not_found_handler;
    extern err_response_handler len_required_handler;
    extern err_response_handler uri_too_long_handler;
    extern err_response_handler unsupported_media_type_handler;
    extern err_response_handler misdirected_req_handler;

    extern err_response_handler not_impl_handler;
}
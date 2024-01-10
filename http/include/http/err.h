#pragma once

#include <http/connection.h>

#include <string>
#include <memory>
#include <iostream>


namespace http
{

    class err_response_handler {
        private:
            std::string reason_;
            unsigned short status_code_;
        public:
            err_response_handler(const std::string &reason, const unsigned short status_code)
                : reason_(reason), status_code_(status_code) { }
            ~err_response_handler()
            {
                std::cout << "destroying err with code: " << status_code_ << std::endl;
            }

            void handle(std::shared_ptr<connection> conn) const;

            const unsigned short status_code(void) const
            {
                return status_code_;
            }

            const std::string& reason(void) const
            {
                return reason_;
            }
    };

    extern err_response_handler bad_req_handler;
    extern err_response_handler not_found_handler;
    extern err_response_handler timeout_handler;
    extern err_response_handler len_required_handler;
    extern err_response_handler uri_too_long_handler;
    extern err_response_handler unsupported_media_type_handler;
    extern err_response_handler misdirected_req_handler;

    extern err_response_handler internal_server_handler; 
    extern err_response_handler not_impl_handler;
}
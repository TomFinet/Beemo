#pragma once

#include <http/conn.h>

#include <string>
#include <memory>
#include <iostream>


namespace beemo
{
    struct err_builder {

        const std::string reason_;
        const unsigned short status_code_;

        err_builder(const std::string &reason,
                    const unsigned short status_code)
            : reason_(reason), status_code_(status_code) { }
        ~err_builder() = default;

        void build(resp*const resp) const;
    };

    extern err_builder bad_req_handler;
    extern err_builder not_found_handler;
    extern err_builder timeout_handler;
    extern err_builder len_required_handler;
    extern err_builder uri_too_long_handler;
    extern err_builder unsupported_media_type_handler;
    extern err_builder misdirected_req_handler;

    extern err_builder internal_server_handler; 
    extern err_builder not_impl_handler;
}
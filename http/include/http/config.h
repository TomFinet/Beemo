#pragma once

#include <transport/config.h>

#include <string>


namespace http
{

    namespace
    {

        /* default network configuration. */
        constexpr auto &default_listening_reg_name = "localhost";

        /* default server message parsing configuration. */
        constexpr unsigned int default_max_req_line_len = 1000;
        constexpr auto &default_default_uri_scheme = "http";
        constexpr auto &default_default_req_content_type = "application/json";

        constexpr auto &default_logger_name = "http";
    }

    /* stores the configuration options chosen by the server. */
    struct config {
        /* Server listens for transport packets addressed to this registered name.
        DONE. */
        std::string listening_reg_name = default_listening_reg_name;
    
        /* Maximum size of a request message's request line.
        DONE. */
        unsigned int max_req_line_len = default_max_req_line_len;

        /* The uri scheme to use by default when uri leaves the scheme unspecified. */
        std::string default_uri_scheme = default_default_uri_scheme;

        /* Default content type when request leaves it unspecified.
        DONE. */
        std::string default_req_content_type = default_default_req_content_type;

        std::string logger_name = default_logger_name;

        /* Transport layer configuration. */
        struct transport::config transport;
    };

    /* read-only configuration. */
    constexpr short supported_http_major_version = 1;
    constexpr short supported_http_minor_version = 1;

}
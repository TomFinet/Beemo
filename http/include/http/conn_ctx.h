#pragma once

#include <sockpp/socket.h>

namespace http
{

    constexpr unsigned int conn_open = 1;
    constexpr unsigned int conn_closed = 2;
    constexpr unsigned int conn_read = 4;
    constexpr unsigned int conn_write = 8;
    constexpr unsigned int conn_default = conn_open | conn_read | conn_write;

    /* Stores the connection context used by the server to process io. */
    struct conn_ctx {
        sockpp::socket_t handle;
        unsigned int status;

        conn_ctx(sockpp::socket_t handle, unsigned int status) : handle(handle), status(status) { }
    };

}
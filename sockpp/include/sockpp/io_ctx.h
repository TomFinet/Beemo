#pragma once

#include <string>
#include <queue>

/* Holds all the context needed to maintain a connection with a remote socket. */

namespace
{
    constexpr int max_rx_len = 100000;
}

namespace sockpp
{
    namespace io
    {
        enum type { rx, tx };
    }

    /* Provides communication from the initialisation of overlapped IO op to its completion. */
    struct io_ctx {

        WSAOVERLAPPED overlapped;
        WSABUF buf_desc;

        char buf[max_rx_len];

        unsigned int bytes_total;
        unsigned int bytes_tx;
        unsigned int bytes_rx;

        io::type type;

        io_ctx(io::type type) : type(type), bytes_total(0), bytes_tx(0), bytes_rx(0), overlapped{0}
        {
            buf_desc.buf = buf;
            buf_desc.len = max_rx_len;
        }

        ~io_ctx() {}
    };
    
}
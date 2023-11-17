#pragma once

#include <string>
#include <queue>

/* Holds all the context needed to maintain a connection with a remote socket. */

namespace
{
    constexpr int max_rx_len = 150;
    constexpr int init_rx_len = 1000;
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

        /* current number of bytes sent. */
        size_t bytes_tx;
        /* total number of bytes to send. */
        size_t bytes_to_tx;

        io::type type;

        io_ctx(io::type type)
            : buf{0}, type(type), bytes_tx(0), bytes_to_tx(0), overlapped{0}
        {
            buf_desc.buf = buf;
            buf_desc.len = max_rx_len;
        }

        ~io_ctx() {}

        /* Writes the data pointed to by the string view into the io buffer.
        Also updates the buffer descriptors and metadata. */
        void write_buf(std::string_view data)
        {
            strncpy_s(buf, data.data(), data.size());
            buf_desc.len = static_cast<unsigned long>(data.size());
            bytes_to_tx = data.size();
            bytes_tx = 0;
        }

    };
    
}
#pragma once

#include <cstddef>
#include <string.h>
#include <string_view>


namespace sockpp
{
    constexpr int max_rx_len = 150;

    namespace io
    {
        enum type { rx, tx };
    }

    /* Provides communication from the initialisation of overlapped IO op to its completion. */
    struct io_ctx {

    #ifdef WIN32
        WSAOVERLAPPED overlapped;
        WSABUF buf_desc;
    #endif

        char buf[max_rx_len];

        /* current number of bytes sent. */
        size_t bytes_tx;
        /* total number of bytes to send. */
        size_t bytes_to_tx;
        /* bytes received. */
        size_t bytes_rx;

        io::type type;

        io_ctx(io::type type);
        ~io_ctx() {}

        /* Writes the data pointed to by the string view into the io buffer.
        Also updates the buffer descriptors and metadata. */
        /* TODO: handle case where data does not fit the buffer. */
        void write_buf(std::string_view data);
    };
    
}
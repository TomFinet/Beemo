#pragma once

#include <cstddef>
#include <string.h>
#include <string_view>

#include <transport/platform.h>


namespace beemo
{
    constexpr unsigned int default_buf_len = 1 << 10;
    enum io_type { RX, TX };

    struct io_buf {

    #ifdef _WIN32
        WSAOVERLAPPED overlapped;
        WSABUF buf_desc;
    #endif

        size_t buf_len_;
        char *buf;

        /* TODO: from io_type, we can deduce if we are dealing with bytes_tx or bytes_rx,
           just have a single member called bytes. */
        /* current number of bytes sent. */
        size_t bytes_tx;
        /* total number of bytes to send. */
        size_t bytes_to_tx;
        /* bytes received. */
        size_t bytes_rx;

        io_type type;

        io_buf(io_type type);
        io_buf(io_type type, const unsigned int buf_len);
        ~io_buf()
        {
            delete[] buf;
        }

        void write_buf(std::string_view data);
    };
}
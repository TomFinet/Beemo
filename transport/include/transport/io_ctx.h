#pragma once

#include <cstddef>
#include <string.h>
#include <string_view>

#include <transport/platform.h>


namespace transport
{
    constexpr unsigned int default_buf_len = 2000;

    namespace io
    {
        enum type { rx, tx };
    }

    /* Provides communication from the initialisation of overlapped IO op to its completion. */
    struct io_ctx {

    #ifdef _WIN32
        WSAOVERLAPPED overlapped;
        WSABUF buf_desc;
    #endif

        size_t buf_len_;
        char *buf;

        /* current number of bytes sent. */
        size_t bytes_tx;
        /* total number of bytes to send. */
        size_t bytes_to_tx;
        /* bytes received. */
        size_t bytes_rx;

        io::type type;

        io_ctx(io::type type);
        io_ctx(io::type type, const unsigned int buf_len);
        ~io_ctx()
        {
            delete[] buf;
        }

        /* Writes the data pointed to by the string view into the io buffer.
        Also updates the buffer descriptors and metadata. */
        /* TODO: handle case where data does not fit the buffer. */
        void write_buf(std::string_view data);
    };
    
}
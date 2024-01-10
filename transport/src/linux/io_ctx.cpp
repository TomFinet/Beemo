#include <transport/io_ctx.h>

#include <algorithm>


namespace transport
{

    io_ctx::io_ctx(io::type type) : type(type), bytes_rx(0), bytes_tx(0), bytes_to_tx(0)
    {
        buf_len_ = default_buf_len;
        buf = new char[buf_len_];
    }
    
    io_ctx::io_ctx(io::type type, const unsigned int buf_len)
        : type(type), bytes_rx(0), bytes_tx(0), bytes_to_tx(0)
    {
        buf_len_ = buf_len;
        buf = new char[buf_len_];
    }

    void io_ctx::write_buf(std::string_view data)
    {
        unsigned int num_bytes = std::min(data.size(), buf_len_);
        strncpy(buf, data.data(), num_bytes);
        bytes_to_tx = num_bytes;
        bytes_tx = 0;
        bytes_rx = 0;
    }
}
#include <transport/io_buf.h>

#include <algorithm>


namespace beemo
{
    io_buf::io_buf(io_type type) : type(type), bytes_rx(0), bytes_tx(0), bytes_to_tx(0)
    {
        buf_len_ = default_buf_len;
        buf = new char[buf_len_];
    }
    
    io_buf::io_buf(io_type type, const unsigned int buf_len)
        : type(type), bytes_rx(0), bytes_tx(0), bytes_to_tx(0)
    {
        buf_len_ = buf_len;
        buf = new char[buf_len_];
    }

    void io_buf::write_buf(std::string_view data)
    {
        if (type == TX) {
            unsigned int num_bytes = std::min(data.size(), buf_len_);
            strncpy(buf, data.data(), num_bytes);
            bytes_to_tx = num_bytes;
            bytes_tx = 0;
            bytes_rx = 0;
        }
    }
}
#include <transport/io_buf.h>

#include <algorithm>


namespace beemo
{
    io_buf::io_buf(io_type type) : type(type), bytes_rx(0), bytes_tx(0), bytes_to_tx(0), overlapped{0}
    {
        buf_len_ = default_buf_len;
        buf = new char[buf_len_];
        buf_desc.len = buf_len_;
        buf_desc.buf = buf;
    }

    io_buf::io_buf(io_type type, const unsigned int buf_len) :
        type(type), bytes_rx(0), bytes_tx(0), bytes_to_tx(0), overlapped{0}
    {
        buf_len_ = buf_len;
        buf = new char[buf_len_];
        buf_desc.len = buf_len_;
        buf_desc.buf = buf;
    }

    void io_buf::write_buf(std::string_view data)
    {
        std::memcpy(buf, data.data(), std::min(data.size(), buf_len_));
        buf_desc.len = static_cast<unsigned long>(data.size());
        bytes_to_tx = data.size();
        bytes_tx = 0;
        bytes_rx = 0;
    }
}
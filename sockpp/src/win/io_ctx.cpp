#include <sockpp/io_ctx.h>


namespace sockpp
{

    io_ctx::io_ctx(io::type type) : type(type), bytes_rx(0), bytes_tx(0), bytes_to_tx(0), overlapped{0}
    {
        buf_desc.buf = buf;
        buf_desc.len = max_rx_len;
    }


    void io_ctx::write_buf(std::string_view data)
    {
        strncpy_s(buf, data.data(), data.size());
        buf_desc.len = static_cast<unsigned long>(data.size());
        bytes_to_tx = data.size();
        bytes_tx = 0;
        bytes_rx = 0;
    }
}
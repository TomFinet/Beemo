#include <transport/io_ctx.h>


namespace transport
{

    io_ctx::io_ctx(io::type type) : type(type), bytes_rx(0), bytes_tx(0), bytes_to_tx(0) { }

    void io_ctx::write_buf(std::string_view data)
    {
        strncpy(buf, data.data(), data.size());
        bytes_to_tx = data.size();
        bytes_tx = 0;
        bytes_rx = 0;
    }
}
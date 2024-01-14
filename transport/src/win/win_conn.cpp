#include "win_conn.h"


namespace beemo
{
    void win_conn::__request_rx(void)
    {
        io_ctx *const rx_io = new io_ctx(io::type::rx);
        skt()->rx(rx_io, 1);
    }

    void win_conn::__request_tx(std::string_view msg)
    {
        io_ctx *const tx_io = new io_ctx(io::type::tx, msg.size());
        tx_io->write_buf(msg);
        skt()->tx(tx_io, 1);
    }
}
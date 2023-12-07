#include <transport/conn_ctx.h>


namespace transport
{

    void conn_ctx::__request_rx()
    {
        io_ctx *const rx_io = new io_ctx(io::type::rx);
        skt->rx(rx_io, 1);
    }

    void conn_ctx::__request_tx(std::string_view msg)
    {
        io_ctx *const tx_io = new io_ctx(io::type::tx);
        tx_io->write_buf(msg);
        skt->tx(tx_io, 1);
    }

}
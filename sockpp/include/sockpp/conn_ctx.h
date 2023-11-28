#pragma once

#include <sockpp/socket.h>
#include <sockpp/io_ctx.h>

#include <string_view>
#include <string>


namespace sockpp
{
    constexpr unsigned int conn_keep_alive = 1;
    constexpr unsigned int conn_rx_closed = 2;
    constexpr unsigned int conn_tx_closed = 4;

    /* Represents a transport layer connection. */
    struct conn_ctx {

        std::unique_ptr<socket> skt;
        unsigned int status;
        /* data rx'ed on this connection is appended to rx_buf. */
        std::string rx_buf;
        
        conn_ctx() : status(conn_keep_alive) { }

        conn_ctx(sockpp::socket_t skt_handle)
            : skt(std::make_unique<sockpp::socket>(skt_handle)),
              status(conn_keep_alive) { }

        void rx(void)
        {
            if (!(status & conn_rx_closed)) {
                skt->rx(new sockpp::io_ctx(sockpp::io::rx), 1);
            }
        }

        void on_rx(std::string_view rx_data)
        {
            rx_buf.append(rx_data);
        }

        void tx(std::string_view msg)
        {
            if (!(status & conn_tx_closed)) {
                sockpp::io_ctx *tx_io = new sockpp::io_ctx(sockpp::io::tx);
                tx_io->write_buf(msg);
                skt->tx(tx_io, 1);
            }
        }

        void clear_rx_buf(void)
        {
            std::string clear_buf;
            rx_buf.swap(clear_buf);
        }

        void close_tx(void)
        {
            if (!(status & conn_tx_closed)) {
                skt->close_tx();
                status |= conn_tx_closed;
            }
        }

        void close_rx(void)
        {
            status |= conn_rx_closed;
        }

        bool keep_alive(void)
        {
            return status & conn_keep_alive;
        }

        ~conn_ctx()
        {
            close_rx();
            close_tx();
            skt->close();
        }
    };

}
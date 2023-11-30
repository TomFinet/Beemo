#pragma once

#include <transport/socket.h>
#include <transport/io_ctx.h>

#include <string_view>
#include <string>
#include <functional>


namespace transport
{
    constexpr unsigned int conn_keep_alive = 1;
    constexpr unsigned int conn_rx_closed = 2;
    constexpr unsigned int conn_tx_closed = 4;


    /* Represents a transport layer connection. */
    struct conn_ctx {

        std::unique_ptr<socket> skt;

        /* Functions to initiate rx/tx over a connection. */
        std::function<void(socket *const)> rx_;
        std::function<void(socket *const, std::string_view)> tx_;

        unsigned int status;

        /* data rx'ed on this connection is appended to rx_buf. */
        std::string rx_buf;
        
        conn_ctx() : status(conn_keep_alive) { }

        conn_ctx(transport::socket_t skt_handle,
                 std::function<void(socket *const)> rx,
                 std::function<void(socket *const, std::string_view)> tx)
            : skt(std::make_unique<socket>(skt_handle)),
              rx_(rx), tx_(tx), status(conn_keep_alive) { }

        void do_rx(void)
        {
            if (!(status & conn_rx_closed)) {
                rx_(skt.get());
            }
        }

        void do_tx(std::string_view msg)
        {
            if (!(status & conn_tx_closed)) {
                tx_(skt.get(), msg);
            }
        }

        void on_rx(std::string_view rx_data)
        {
            rx_buf.append(rx_data);
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
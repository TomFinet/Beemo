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
    class conn_ctx {
        
        private:
            std::unique_ptr<socket> skt;
            io_queue_t queue_handle_;

            unsigned int status;

            /* data rx'ed on this connection is appended to rx_buf. */
            std::string rx_buf_;

            void __request_rx(void);
            void __request_tx(std::string_view msg);
        
        public:
            conn_ctx() : status(conn_keep_alive) { }

            conn_ctx(socket_t skt_handle, io_queue_t queue_handle)
                : skt(std::make_unique<socket>(skt_handle)), queue_handle_(queue_handle), status(conn_keep_alive) { }

            void request_rx(void)
            {
                if (!(status & conn_rx_closed)) {
                    __request_rx();
                }
            }

            void request_tx(std::string_view msg)
            {
                if (!(status & conn_tx_closed)) {
                    __request_tx(msg);
                }
            }

            void on_rx(std::string_view rx_data)
            {
                rx_buf_.append(rx_data);
            }

            void clear_rx_buf(void)
            {
                std::string clear_buf;
                rx_buf_.swap(clear_buf);
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

            std::string& rx_buf(void)
            {
                return rx_buf_;
            }

            socket_t skt_handle(void)
            {
                return skt->handle();
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
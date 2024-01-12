#pragma once

#include <transport/platform.h>
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
            std::unique_ptr<socket> skt_;
            unsigned int status_;

            /* data rx'ed on this connection is appended to rx_buf. */
            std::string rx_buf_;

            virtual void __request_rx(void) { }
            virtual void __request_tx(std::string_view msg) { }
        
        public:
            conn_ctx() : status_(conn_keep_alive) { }
            conn_ctx(socket_t skt_handle)
                : skt_(std::make_unique<socket>(skt_handle)), status_(0) { }
            ~conn_ctx() { }

            void request_rx(void)
            {
                if (!(status_ & conn_rx_closed)) {
                    __request_rx();
                }
            }

            void request_tx(std::string_view msg)
            {
                if (!(status_ & conn_tx_closed)) {
                    __request_tx(msg);
                }
            }

            void on_rx(std::string_view rx_data)
            {
                rx_buf_.append(rx_data);
            }

            void clear_rx_buf(void)
            {
                rx_buf_.clear();
            }

            void close_tx(void)
            {
                if (!(status_ & conn_tx_closed)) {
                    skt_->close_tx();
                    status_ |= conn_tx_closed;
                }
            }

            void close_rx(void)
            {
                status_ |= conn_rx_closed;
            }

            std::string& rx_buf(void)
            {
                return rx_buf_;
            }

            socket* skt(void)
            {
                return skt_.get();
            }

            socket_t skt_handle(void)
            {
                return skt_->handle();
            }

            unsigned int status(void)
            {
                return status_;
            }

            void toggle_status(unsigned int flag, bool active)
            {
                if (active) {
                    status_ |= flag;
                }
                else {
                    status_ &= ~flag;
                }
            }
    };

}
#pragma once

#include <http/msg.h>

#include <transport/io_buf.h>
#include <transport/socket.h>

#include <memory>
#include <functional>
#include <string_view>


namespace beemo
{
    /* TODO: this should probably be defined in the events module. */
    using event_io = std::function<void(socket_t)>;

    constexpr unsigned int conn_keep_alive = 1;
    constexpr unsigned int conn_rx_closed = 2;
    constexpr unsigned int conn_tx_closed = 4;
    constexpr unsigned int conn_rtx_closed = 8;

    struct conn {

        unsigned int status_;
        unsigned int parsed_to_idx_;

        std::unique_ptr<socket> skt_;
        std::string_view io_rx_;
        std::unique_ptr<io_buf> io_tx_;

        event_io on_rx_;
        event_io on_tx_;
        event_io on_close_;
        event_io on_timeout_;

        std::unique_ptr<req> req_;
        std::unique_ptr<resp> resp_;
        
        conn(const socket_t sktfd);
        ~conn() = default;
        
        void close_rx(void);
        void close_tx(void);
        void close_rtx(void);

        void reset_state(void);
        void prepare_tx(void);
        void prepare_tx(std::string_view tx);
    };
}
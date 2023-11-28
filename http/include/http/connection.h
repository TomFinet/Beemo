#pragma once

/* gives us a view of the data exchanged on the connection at the http layer. */

#include <http/msg.h>

#include <sockpp/socket.h>
#include <sockpp/io_ctx.h>
#include <sockpp/conn_ctx.h>

#include <memory>


namespace http
{

    /* Stores http connection request and response state. */
    struct connection {

        /* TODO: why are request and response unique_ptr? Could they not just be members? */
        std::unique_ptr<req> request;
        std::unique_ptr<response> res;
        sockpp::conn_ctx transport_conn; 
        /* index we have parsed up to, so that next parse can avoid redoing the same work. */
        size_t parsed_to_idx;

        connection(sockpp::socket_t skt_handle)
            : request(std::make_unique<req>()),
              res(std::make_unique<response>()),
              transport_conn(skt_handle),
              parsed_to_idx(0) { }

        ~connection() { }

        void rx(void)
        {
            transport_conn.rx();
        }

        void tx(void)
        {
            transport_conn.tx(res->to_str());
        }

        void reset_for_next(void)
        {
            parsed_to_idx = 0;
            transport_conn.clear_rx_buf();

            std::unique_ptr<req> clear_req = std::make_unique<req>();
            request.swap(clear_req);
        }

        void close_tx(void)
        {
            transport_conn.close_tx();
        }

        void close_rx(void)
        {
            transport_conn.close_rx();
        }

        bool keep_alive(void)
        {
            return transport_conn.keep_alive();
        }
    };

}
#pragma once

#include <memory>

#include <sockpp/socket.h>
#include <sockpp/io_ctx.h>

#include <http/msg.h>

namespace http
{

    constexpr unsigned int conn_keep_alive = 1;
    constexpr unsigned int conn_rx_closed = 2;
    constexpr unsigned int conn_tx_closed = 4;

    /* Stores the connection context used by the server to process io. */
    struct conn_ctx {
        std::unique_ptr<sockpp::socket> skt;
        /* only one request and response at any one time for a connection. */
        std::unique_ptr<req> request;
        std::unique_ptr<response> res;
        unsigned int status;

        /* transport packets do not necessarily contain the entire
        HTTP request, hence we may need to glue multiple transport packets together. */
        std::string reassembly_buf;
        /* index we have parsed up to, so that next parse can avoid redoing the same work. */
        size_t parsed_to_idx;

        conn_ctx(std::unique_ptr<sockpp::socket> skt)
            : skt(std::move(skt)), status(conn_keep_alive), request(std::make_unique<req>()), parsed_to_idx(0) { }


        void rx(void)
        {
            if (!(status & conn_rx_closed)) {
                skt->rx(new sockpp::io_ctx(sockpp::io::rx), 1);
            }
        }

        void tx(std::string_view msg)
        {
            if (!(status & conn_tx_closed)) {
                /* TODO: Might be leaking these when connection is closed, and client later sends packet to us. */
                sockpp::io_ctx *tx_io = new sockpp::io_ctx(sockpp::io::tx);
                tx_io->write_buf(msg);
                skt->tx(tx_io, 1);
            }
        }

        /* when we have parsed a request or encountered an error,
        we call reset_req to start the next request fresh. */
        void reset_for_next(void)
        {
            reassembly_buf.clear();
            reassembly_buf.shrink_to_fit();

            parsed_to_idx = 0;

            /* maybe just swap request with a new request. */
            request->fields.clear();
            request->content.clear();
            request->content.shrink_to_fit();
            request->err = nullptr;
            request->uri = {0};
            request->parse_state = start_line;
            std::vector<encoding_t> empty_encoding_list;
            std::swap(request->transfer_encodings, empty_encoding_list);
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

        ~conn_ctx()
        {
            close_tx();
            skt->close();
            status |= conn_rx_closed;
        }
    };

}
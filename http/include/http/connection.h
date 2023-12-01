#pragma once

/* gives us a view of the data exchanged on the connection at the http layer. */

#include <http/msg.h>

#include <transport/conn_ctx.h>

#include <memory>


namespace http
{

    struct connection {
        std::unique_ptr<req> req_;
        std::unique_ptr<response> res_;
        std::shared_ptr<transport::conn_ctx> transport_conn_; 

        /* index we have parsed up to, so that next parse can avoid redoing the same work. */
        size_t parsed_to_idx_;

        connection(std::shared_ptr<transport::conn_ctx> transport_conn)
            : req_(std::make_unique<req>()),
              res_(std::make_unique<response>()),
              transport_conn_(transport_conn),
              parsed_to_idx_(0) { }

        ~connection() { }

        void rx(void)
        {
            transport_conn_->do_rx();
        }

        void tx(void)
        {
            transport_conn_->do_tx(res_->to_str());
        }

        void close_rx(void)
        {
            transport_conn_->close_rx();
        }
        
        void close_tx(void)
        {
            transport_conn_->close_tx();
        }
        
        void reset_for_next(void)
        {
            parsed_to_idx_ = 0;
            transport_conn_->clear_rx_buf();

            std::unique_ptr<req> clear_req = std::make_unique<req>();
            req_.swap(clear_req);
        }

        bool keep_alive(void)
        {
            return transport_conn_->keep_alive();
        }
    };

}
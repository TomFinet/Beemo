#include <http/server.h>
#include <http/parser.h>
#include <http/err.h>
#include <http/routing.h>


namespace http
{

    server::server(const config &config) : config_(config)
    {
        logger_ = spdlog::stdout_color_mt(config_.logger_name);

        transport_server_ = std::make_unique<transport::server>(
            [this](std::shared_ptr<transport::conn_ctx> conn) { this->handle_connection(conn); },
            [this](transport::socket_t skt_handle) { this->handle_rx(skt_handle); },
            [this](transport::socket_t skt_handle) { this->handle_tx(skt_handle); },
            [this](transport::socket_t skt_handle) { this->remove_conn(skt_handle); },
            config_.transport
        );
    }

    void server::start(void)
    {
        logger_->info("Starting HTTP server.");
        transport_server_->start(); 
    }

    void server::handle_connection(std::shared_ptr<transport::conn_ctx> transport_conn)
    {
        /* TODO: register connection on_rx and on_tx functions to map to the http server handle_rx and handle_tx. */
        add_conn(std::make_shared<connection>(transport_conn));
        auto conn = connections_[transport_conn->skt->handle()]; 
        conn->rx();
    }


    /* Responsible for handling transport layer rx completion. */
    void server::handle_rx(transport::socket_t skt_handle)
    {
        std::shared_ptr<connection> conn = connections_[skt_handle];

        if (conn->req_->is_parsing_headers()) {
            conn->parsed_to_idx_ = parse_headers(conn->transport_conn_->rx_buf, conn->req_.get(), {conn->parsed_to_idx_, &config_});
        }

        if (conn->req_->is_parsing_headers()) {
            goto rx; /* Issue an rx req for remaining header data. */
        }

        if (!conn->req_->has_err()) {

            validate(conn->req_.get(), config_);
            if (conn->req_->has_err()) {
                goto err;
            }
            
            parse_content(conn->transport_conn_->rx_buf.substr(conn->parsed_to_idx_), conn->req_.get());

            if (conn->req_->is_parsing_incomplete()) {
                goto rx; /* Issue a new recv request to get the rest of the message content. */
            }
        }

        if (conn->req_->has_err()) {
            goto err;
        }

        conn->res_ = route_to_resource_handler(conn->req_.get());
        if (conn->req_->has_err()) {
            goto err;
        }

        conn->reset_for_next();
        conn->tx();
        return;

    err:
        conn->req_->err->handle(conn, logger_);
        conn->close_tx();
        return;
    rx:
        conn->rx();
    }
    
    void server::handle_tx(transport::socket_t skt_handle)
    {
        std::shared_ptr<connection> conn = connections_[skt_handle];

        if (!conn->keep_alive()) {
            remove_conn(conn->transport_conn_->skt->handle());
        }
        else {
            conn->rx();
        }
    }

    void server::add_conn(std::shared_ptr<connection> conn)
    {
        std::unique_lock<std::mutex> lock(conn_mutex_);
        connections_[conn->transport_conn_->skt->handle()] = conn;
    }

    void server::remove_conn(transport::socket_t skt_handle)
    {
        std::unique_lock<std::mutex> lock(conn_mutex_);
        connections_.erase(skt_handle);
    }

}

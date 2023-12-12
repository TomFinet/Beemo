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
            [this](transport::socket_t skt_handle) { this->handle_timeout(skt_handle); },
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
        add_conn(std::make_shared<connection>(transport_conn));
        auto conn = connections_[transport_conn->skt_handle()]; 
        conn->rx();
    }

    void server::handle_rx(transport::socket_t skt_handle)
    {
        std::shared_ptr<connection> conn = connections_[skt_handle];
        logger_->info(conn->transport_conn_->rx_buf());
        if (conn->req_->is_parsing_headers()) {
            conn->parsed_to_idx_ = parse_headers(conn->transport_conn_->rx_buf(), conn->req_.get(), {conn->parsed_to_idx_, &config_});
        }

        if (conn->req_->is_parsing_headers()) {
            goto rx; /* Issue an rx req for remaining header data. */
        }

        if (!conn->req_->has_err()) {
            parse_content(conn->transport_conn_->rx_buf().substr(conn->parsed_to_idx_), conn->req_.get(), config_);
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
        logger_->error("Error {0:d} {1}", conn->req_->err->status_code(), conn->req_->err->reason());
        conn->req_->err->handle(conn);
        conn->close_tx();
        return;
    rx:
        conn->rx();
    }
    
    void server::handle_tx(transport::socket_t skt_handle)
    {
        std::shared_ptr<connection> conn = connections_[skt_handle];

        if (!conn->keep_alive()) {
            remove_conn(conn->transport_conn_->skt_handle());
        }
        else {
            conn->rx();
        }
    }

    void server::handle_timeout(transport::socket_t skt_handle)
    {
        std::shared_ptr<connection> conn = connections_[skt_handle];
        timeout_handler.handle(conn);
        conn->close_tx();
        logger_->error("Timeout on skt {0:d}", skt_handle);
    }

    void server::add_conn(std::shared_ptr<connection> conn)
    {
        std::unique_lock<std::mutex> lock(conn_mutex_);
        connections_[conn->transport_conn_->skt_handle()] = conn;
    }

    void server::remove_conn(transport::socket_t skt_handle)
    {
        std::unique_lock<std::mutex> lock(conn_mutex_);
        connections_.erase(skt_handle);
    }

}

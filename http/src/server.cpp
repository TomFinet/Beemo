#include <http/server.h>
#include <http/parser.h>
#include <http/err.h>
#include <http/routing.h>

#include <utils/terminal_art.h>


namespace beemo
{
    server::server(const config &config) : config_(config)
    {
        socket::startup();
        spdlog::set_pattern("[%H:%M:%S %z] [thread %t] %v");

        logger_ = spdlog::stdout_color_mt(config_.logger_name);
        logger_->set_level(spdlog::level::err);
        evloop_ = std::make_unique<evloop>(config_.max_concurrent_connections, config_.processing_timeout_ms);
        workers_ = std::make_unique<pool>(config_.num_req_handler_threads);
    }

    server::~server()
    {
        {
            std::unique_lock<std::mutex> lock(conn_mtx_);
            for (const auto& [_, conn] : conns_) {
                on_close(conn);
            }
        }
        socket::cleanup();
    }

    void server::start(void)
    {
        logger_->info("Starting Beemo HTTP/1.1 web server.");
        logger_->info(beemo_terminal_art);
        
        for (int i = 0; i < config_.num_req_handler_threads; i++) {
            workers_->submit([this]() { evloop_->loop(config_.max_events_per_thread, -1); });
        }

        acc_.open(config_.listening_ip, config_.listening_port, config_.max_connection_backlog,
                config_.max_linger_sec, config_.rx_buf_len, config_.idle_connection_timeout_ms);

        for (;;) {
            socket_t sktfd;
            {
                std::unique_lock<std::mutex> lock(conn_mtx_);
                conn_cond_.wait(lock, [this]{ return conns_.size() < config_.max_concurrent_connections; });
            }
            try {
                sktfd = acc_.accept();
                on_conn(sktfd);
            }
            catch (transport_err) {
                logger_->error("[skt {0}] transport error {1}", sktfd, errno);
                return;
            }
        }
    }
    
    void server::on_conn(socket_t sktfd)
    {
        std::shared_ptr<conn> c = std::make_shared<conn>(sktfd);
        c->skt_->blocking(false);
        c->on_rx_ = [this](std::shared_ptr<conn> conn) { on_rx(conn); };
        c->on_tx_ = [this](std::shared_ptr<conn> conn) { on_tx(conn); };
        c->on_close_ = [this](std::shared_ptr<conn> conn) { on_close(conn); };
        c->on_timeout_ = [this](std::shared_ptr<conn> conn) { on_timeout(conn); };
        {
            std::unique_lock<std::mutex> lock(conn_mtx_);
            conns_[sktfd] = c;
        }
        evloop_->reg(c, EV_IN);
    }

    void server::on_rx(std::shared_ptr<conn> conn)
    {
        if (conn->req_->is_parsing_headers()) {
            conn->parsed_to_idx_ = parse_headers(conn->io_rx_, conn->req_.get(), {conn->parsed_to_idx_, &config_});
        }

        if (conn->req_->is_parsing_headers()) {
            goto partial;
        }

        if (!conn->req_->has_err()) {
            parse_content(conn->io_rx_.substr(conn->parsed_to_idx_), conn->req_.get(), config_);
            if (conn->req_->is_parsing_incomplete()) {
                goto partial;
            }
        }

        if (conn->req_->has_err()) {
            goto err;
        }

        conn->resp_ = route_to_resource_handler(conn->req_.get());
        if (conn->req_->has_err()) {
            goto err;
        }

        conn->prepare_tx();
        evloop_->reg(conn, EV_OUT);
        return;

    partial:
        evloop_->reg(conn, EV_IN); 
        return;

    err:
        logger_->error("[skt {2}] HTTP error {0} {1}",
                        conn->req_->err->status_code_,
                        conn->req_->err->reason_, conn->skt_->handle_);
        conn->req_->err->build(conn->resp_.get());
        conn->status_ &= ~conn_keep_alive;
        conn->prepare_tx(); 
        evloop_->reg(conn, EV_OUT); 
    }
    
    void server::on_tx(std::shared_ptr<conn> conn)
    {
        if (conn->status_ & conn_keep_alive) {
            conn->reset_state();
            evloop_->reg(conn, EV_IN);
            return;
        }
        on_close(conn);
    }

    void server::on_close(std::shared_ptr<conn> conn)
    {
        {
            std::unique_lock<std::mutex> lock(conn_mtx_);
            conns_.erase(conn->skt_->handle_);
            logger_->info("[skt {0}] connection closed {1}", conn->skt_->handle_, conns_.size());
        }
        conn_cond_.notify_one();
    }
    
    void server::on_timeout(std::shared_ptr<conn> conn)
    {
        timeout_handler.build(conn->resp_.get());
        on_close(conn);
        logger_->error("Timeout on skt {0}", conn->skt_->handle_);
    }
}

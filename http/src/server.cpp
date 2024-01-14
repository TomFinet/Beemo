#include <http/server.h>
#include <http/parser.h>
#include <http/err.h>
#include <http/routing.h>


namespace beemo
{
    server::server(const config &config) : config_(config)
    {
        socket::startup();
        spdlog::set_pattern("[%H:%M:%S %z] [thread %t] %v");

        logger_ = spdlog::stdout_color_mt(config_.logger_name);
        evloop_ = std::make_unique<evloop>(config_.max_concurrent_connections);
        workers_ = std::make_unique<pool>(config_.num_req_handler_threads, config_.processing_timeout_sec);
    }

    server::~server()
    {
        {
            std::unique_lock<std::mutex> lock(conn_mtx_);
            for (const auto& [sktfd, _] : conns_) {
                on_close(sktfd);
            }
        }
        socket::cleanup();
    }

    void server::start(void)
    {
        /* TODO: would be cool to get some Beemo terminal art here... */
        logger_->info("Starting Beemo HTTP/1.1 web server.");
        
        for (int i = 0; i < config_.num_req_handler_threads; i++) {
            workers_->submit([this]() { evloop_->loop(config_.max_events_per_thread, -1); });
        }

        acc_.open(config_.listening_ip, config_.listening_port, config_.max_connection_backlog,
                config_.max_linger_sec, config_.rx_buf_len, config_.idle_connection_timeout_ms);

        for(;;) {
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
        c->on_rx_ = [this](socket_t sktfd) { on_rx(sktfd); };
        c->on_tx_ = [this](socket_t sktfd) { on_tx(sktfd); };
        c->on_close_ = [this](socket_t sktfd) { on_close(sktfd); };
        c->on_timeout_ = [this](socket_t sktfd) { on_timeout(sktfd); };
        {
            std::unique_lock<std::mutex> lock(conn_mtx_);
            conns_[sktfd] = c;
        }
        evloop_->reg(c, EV_IN);
        logger_->info("[skt {0}] connection", sktfd);
    }

    void server::on_rx(socket_t sktfd)
    {
        std::shared_ptr<conn> conn = get_conn(sktfd);
        std::string_view rx = conn->io_rx_;

        if (conn->req_->is_parsing_headers()) {
            conn->parsed_to_idx_ = parse_headers(rx, conn->req_.get(), {conn->parsed_to_idx_, &config_});
        }

        if (conn->req_->is_parsing_headers()) {
            goto partial;
        }

        if (!conn->req_->has_err()) {
            parse_content(rx.substr(conn->parsed_to_idx_), conn->req_.get(), config_);
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
                        conn->req_->err->reason_, sktfd);
        conn->req_->err->build(conn->resp_.get());
        conn->status_ &= ~conn_keep_alive;
        conn->prepare_tx(); 
        evloop_->reg(conn, EV_OUT); 
    }
    
    void server::on_tx(socket_t sktfd)
    {
        std::shared_ptr<conn> conn = get_conn(sktfd);
        if (conn->status_ & conn_keep_alive) {
            conn->reset_state();
            evloop_->reg(conn, EV_IN);
            return;
        }
        on_close(sktfd);
    }

    void server::on_close(socket_t sktfd)
    {
        std::unique_lock<std::mutex> lock(conn_mtx_);
        conns_.erase(sktfd);
        conn_cond_.notify_one();
        logger_->info("[skt {0}] connection closed {1}", sktfd, conns_.size());
    }
    
    void server::on_timeout(socket_t sktfd)
    {
        timeout_handler.build(get_conn(sktfd)->resp_.get());
        on_close(sktfd);
        logger_->error("Timeout on skt {0:d}", sktfd);
    }

    std::shared_ptr<conn> server::get_conn(socket_t sktfd)
    {
        std::unique_lock<std::mutex> lock(conn_mtx_);
        return conns_[sktfd];
    }
}

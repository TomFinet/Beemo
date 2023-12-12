/* Class wrapping the IO queue used to send and receive packets.
   On windows this creates an IO completion port. */

#pragma once

#include <stdexcept>
#include <vector>
#include <mutex>
#include <memory>
#include <functional>
#include <thread>

#include <transport/platform.h>
#include <transport/conn_ctx.h>
#include <transport/acceptor.h>
#include <transport/socket.h>
#include <transport/config.h>
#include <transport/err.h>

#include <threadpool/pool.h>

#include <utils/timeout.h>
#include <utils/timeout_err.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace transport
{

    using io_cb_t = std::function<void(socket_t)>;
    using conn_cb_t = std::function<void(std::shared_ptr<conn_ctx>)>;

    class server {
         
        private:
            conn_cb_t on_conn_;
            io_cb_t on_rx;
            io_cb_t on_tx;
            io_cb_t on_client_close;
            io_cb_t on_timeout_;

            io_queue_t queue_handle_;

            acceptor acc_;

            std::unique_ptr<threadpool::pool> io_workers;

            struct config config_; 
            std::shared_ptr<spdlog::logger> logger_;

            std::unordered_map<socket_t, std::shared_ptr<conn_ctx>> conns_;
            std::mutex conn_mutex_;
            std::condition_variable conn_condition_;

        #ifdef __linux__
            struct epoll_events* ready_events_;
            std::unordered_map<socket_t, std::unique_ptr<epoll_event>> events_;
            std::unordered_map<socket_t, std::unique_ptr<io_ctx>> outgoing_io_;
            std::mutex events_mutex_;
            std::mutex outgoing_mutex_;
        #endif

            void register_socket(const socket_t handle, conn_ctx *const conn); 

        public:

            server(conn_cb_t on_conn, io_cb_t on_rx, io_cb_t on_tx, io_cb_t on_client_close,
                   io_cb_t on_timeout, const config &config);

            ~server()
            {
                transport::socket::cleanup();
                /* TODO: we probs need to close the queue_handle file. */
            }

            void start(void)
            {
                for (int i = 0; i < config_.num_req_handler_threads; i++) {
                    io_workers->submit([this]() { this->run_event_loop(); });
                }

                acc_.open(config_.listening_ip, config_.listening_port, config_.max_connection_backlog,
                        config_.max_linger_sec, config_.rx_buf_len, config_.idle_connection_timeout_ms);
                        
                while (true) {
                    try {
                        {
                            std::unique_lock<std::mutex> lock(conn_mutex_);
                            conn_condition_.wait(lock, [this]{ return conns_.size() < config_.max_concurrent_connections; });
                        }
                        transport::socket_t skt_handle = acc_.accept();
                        std::shared_ptr<conn_ctx> conn = std::make_shared<conn_ctx>(skt_handle, queue_handle_);

                        logger_->info("Connections: {0:d}", conns_.size());

                        add_conn(conn);
                        register_socket(skt_handle, conn.get());
                        logger_->info("New connection established.");
                        on_conn_(conn);
                    }
                    catch (transport_err) {
                        /* TODO: do some sort of error logging. But ultimately, we don't want
                        errors relating to one connection to affect other connections. */
                        logger_->error("Transport error encountered...");
                    }
                }
            }

            void run_event_loop(void);
            
            void add_conn(std::shared_ptr<conn_ctx> conn)
            {
                std::unique_lock<std::mutex> lock(conn_mutex_);
                conns_[conn->skt_handle()] = conn;
            }

            void remove_conn(transport::socket_t skt_handle)
            {
                std::unique_lock<std::mutex> lock(conn_mutex_);
                conns_.erase(skt_handle);
                conn_condition_.notify_one();
            }
            
            void cb_with_timeout(const io_cb_t cb, const socket_t skt_handle)
            {
                try {
                    cb(skt_handle); /* TODO: maybe this should just be in conn->on_rx, that way we can customise io handling per connection, instead of per server. */
                }
                catch (utils::timeout_err) {
                    on_timeout_(skt_handle);
                }
            }
    };

}
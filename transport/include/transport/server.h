#pragma once

#include <stdexcept>
#include <vector>
#include <mutex>
#include <memory>
#include <functional>
#include <thread>
#include <array>
#include <string_view>

#include <transport/platform.h>
#include <transport/conn_ctx.h>
#include <transport/acceptor.h>
#include <transport/socket.h>
#include <transport/config.h>
#include <transport/err.h>

#include <threadpool/pool.h>

#include <utils/timeout.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace transport
{

    using io_cb_t = std::function<void(socket_t)>;
    using conn_cb_t = std::function<void(std::shared_ptr<conn_ctx>)>;
    
    class epoll_ctx;

    class server {
         
        private:
            /* event callbacks. */
            conn_cb_t on_conn_;
            io_cb_t on_rx;
            io_cb_t on_tx;
            io_cb_t on_client_close;
            io_cb_t on_timeout_;

            config config_; 
            acceptor acc_;

            std::unique_ptr<threadpool::pool> io_workers;
            std::shared_ptr<spdlog::logger> logger_;

            /* connection map. */
            std::unordered_map<socket_t, std::shared_ptr<conn_ctx>> conns_;
            std::mutex conn_mutex_;
            std::condition_variable conn_condition_;

        #if defined(__linux__)
            std::shared_ptr<epoll_ctx> epoll_ctx_;
        #elif defined(WIN32)
            io_queue_t queue_handle_;
	#endif

            void register_socket(const socket_t handle, std::shared_ptr<conn_ctx> conn); 

        public:

            server(conn_cb_t on_conn, io_cb_t on_rx, io_cb_t on_tx, io_cb_t on_client_close,
                   io_cb_t on_timeout, const config &config);

            ~server()
            {
                {
                    std::unique_lock<std::mutex> lock(conn_mutex_);
                    for (const auto& [skt_handle, _] : conns_) {
                        remove_conn(skt_handle);
                    }
                }
                socket::cleanup();
            }
    
            void start(void)
            {
                for (int i = 0; i < config_.num_req_handler_threads; i++) {
                    io_workers->submit([this]() { this->run_event_loop(); });
                }

                acc_.open(config_.listening_ip, config_.listening_port, config_.max_connection_backlog,
                        config_.max_linger_sec, config_.rx_buf_len, config_.idle_connection_timeout_ms);
                        
                while (true) {
                    std::shared_ptr<conn_ctx> conn;
                    try {
                        {
                            std::unique_lock<std::mutex> lock(conn_mutex_);
                            conn_condition_.wait(lock, [this]{ return conns_.size() < config_.max_concurrent_connections; });
                        }
                        socket_t skt_handle = acc_.accept();
                        conn = add_conn(skt_handle);
                        conn->skt()->blocking(false);
                        register_socket(skt_handle, conn);
                        on_conn_(conn);
                    }
                    catch (transport_err) {
                        logger_->error("[skt {0}] transport error {1}", conn->skt_handle(), conn->skt()->get_last_error());
                        return;
                    }
                }
            }

            void remove_conn(socket_t skt_handle)
            {
                on_client_close(skt_handle);
                {
                    std::unique_lock<std::mutex> lock(conn_mutex_);
                    conns_.erase(skt_handle);
                }
                logger_->info("[skt {0}] connection removed: {1}", skt_handle, conns_.size());
                /* notify main thread #conns decreased. */
                conn_condition_.notify_one();
            }
            
            void cb_with_timeout(const io_cb_t cb, const socket_t skt_handle)
            {
                /*
                bool timeout = utils::run(cb, config_.processing_timeout_sec, skt_handle);
                if (timeout) {
                    on_timeout_(skt_handle);
                }
                */
                cb(skt_handle);
            }

            void handle_in(std::shared_ptr<conn_ctx> conn);
            void handle_out(std::shared_ptr<conn_ctx> conn);
            void run_event_loop(void);
            std::shared_ptr<conn_ctx> add_conn(socket_t skt_handle);
    };

}

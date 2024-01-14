#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <mutex>
#include <unordered_map>

#include <http/msg.h>
#include <http/conn.h>
#include <http/config.h>

#include <transport/acceptor.h>
#include <threadpool/pool.h>
#include <ev/evloop.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace beemo
{
    class server {
        private:
            config config_; 
            acceptor acc_;

            std::shared_ptr<spdlog::logger> logger_;
            std::unique_ptr<pool> workers_;
            std::unique_ptr<evloop> evloop_;

            std::unordered_map<socket_t, std::shared_ptr<conn>> conns_;
            std::mutex conn_mtx_;
            std::condition_variable conn_cond_;

            void on_conn(socket_t sktfd);
            void on_rx(socket_t sktfd);
            void on_tx(socket_t sktfd);
            void on_close(socket_t sktfd);
            void on_timeout(socket_t skt_handle);
            
            std::shared_ptr<conn> get_conn(socket_t sktfd);

        public:
            server(const config &config);
            ~server();

            void start(void); 
    };
}
#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <mutex>
#include <unordered_map>

#include <http/msg.h>
#include <http/connection.h>
#include <http/config.h>

#include <transport/server.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace http
{
    
    class server {

        public:

            server(const config &config);
            ~server() { }

            void start(void); 

        private:

            const struct config config_;
            std::shared_ptr<spdlog::logger> logger_;
            std::unique_ptr<transport::server> transport_server_;

            std::unordered_map<transport::socket_t, std::shared_ptr<connection>> connections_;
            std::mutex conn_mutex_;

        private:

            void add_conn(std::shared_ptr<connection> conn);
            void remove_conn(transport::socket_t skt_handle);

            void handle_connection(std::shared_ptr<transport::conn_ctx> conn);
            void handle_rx(transport::socket_t handle);
            void handle_tx(transport::socket_t handle);
            void handle_timeout(transport::socket_t skt_handle);
    };

}
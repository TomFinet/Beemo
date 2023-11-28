#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

#include <http/msg.h>
#include <http/connection.h>
#include <http/config.h>

#include <sockpp/io_ctx.h>
#include <sockpp/acceptor.h>
#include <sockpp/io_queue.h>

#include <threadpool/pool.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace http
{
    
    class server {

        public:

            server(const struct config &config);
            ~server();

            void start(void); 

        private:

            const struct config config_;
            std::shared_ptr<spdlog::logger> logger_;

            sockpp::acceptor acc;

            std::unique_ptr<sockpp::io_queue> io_queue;
            std::unique_ptr<threadpool::pool> io_workers;

            /* When a new connection is made, a conn_ctx object is created.
            A ptr to it is stored here until the connection is closed.
            In case the connection is closed when still processing a previous io request
            on the connection, we make this a shared_ptr.
            We will need to synchronise this. Maybe we should have a fixed array for the max number of connections.
            And hand out a unique index to each new connection, that way we can have multiple writes without synchronisation. */
            std::unordered_map<sockpp::socket_t, std::shared_ptr<connection>> connections;
            std::mutex conn_mutex;
            std::condition_variable condition;

        private:

            void add_conn(std::shared_ptr<connection> conn);
            void remove_conn(sockpp::socket_t skt_handle);

            void handle_rx(sockpp::socket_t handle);
            void handle_tx(sockpp::socket_t handle);
    };

}
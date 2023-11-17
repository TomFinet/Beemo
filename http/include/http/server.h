#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

#include <http/msg.h>
#include <http/conn_ctx.h>

#include <sockpp/io_ctx.h>
#include <sockpp/acceptor.h>
#include <sockpp/io_queue.h>

#include <threadpool/pool.h>


namespace http {
    
    /* Used as the thread limit when creating a completion port. A value
    of 0 is used to indicate: as many threads as this machine has processors. */
    constexpr int nproc = 0;
    constexpr int default_port = 80;
    constexpr auto &default_scheme = "http";

    class server {

        public:

            server(int max_msg_len, int max_conn, int max_backlog, int timeout_ms, int thread_num,
                   int listening_port, const std::string &listening_ip);
            ~server();

            void start(void); 

        private:

            std::string default_scheme_;
            int default_port_;

            int max_msg_len;
            int active_conn;
            int max_conn;
            int max_backlog;
            int timeout_ms;
            int thread_num;

            int listening_port;
            std::string listening_ip;

            sockpp::acceptor acc;

            std::unique_ptr<sockpp::io_queue<conn_ctx>> io_queue;
            std::unique_ptr<threadpool::pool> io_workers;

            /* When a new connection is made, a conn_ctx object is created.
            A ptr to it is stored here until the connection is closed.
            In case the connection is closed when still processing a previous io request
            on the connection, we make this a shared_ptr.
            We will need to synchronise this. Maybe we should have a fixed array for the max number of connections.
            And hand out a unique index to each new connection, that way we can have multiple writes without synchronisation. */
            std::unordered_map<sockpp::socket_t, std::shared_ptr<conn_ctx>> connections;
            std::mutex conn_mutex;
            std::condition_variable condition;

        private:

            void add_conn(std::shared_ptr<conn_ctx> conn);
            void remove_conn(sockpp::socket_t conn_handle);

            void handle_rx(conn_ctx *conn_ptr, std::unique_ptr<sockpp::io_ctx> io);
            void handle_tx(conn_ctx *conn_ptr, std::unique_ptr<sockpp::io_ctx> io);
            void handle_close(conn_ctx *conn);
    };

}
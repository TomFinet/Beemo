#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <http/conn_ctx.h>

#include <sockpp/io_ctx.h>
#include <sockpp/acceptor.h>
#include <sockpp/io_queue.h>

#include <threadpool/pool.h>


namespace http {
    
    /* Used as the thread limit when creating a completion port. A value
    of 0 is used to indicate: as many threads as this machine has processors. */
    constexpr int nproc = 0;

    class server {

        public:

            server(int max_msg_len, int max_conn, int max_backlog, int timeout_ms, int thread_num,
                   int listening_port, std::string &listening_ip);
            ~server();

            void start(void); 

        private:

            int default_port = 80;
            std::string default_scheme = "http";
            
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

            std::vector<std::shared_ptr<conn_ctx>> connections;

        private:

            void handle_io(std::shared_ptr<conn_ctx> conn, std::shared_ptr<sockpp::io_ctx> ctx);
            void handle_close(void);
            void validate(std::shared_ptr<struct req> req);
            
    };

}
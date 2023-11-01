#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <sockpp/acceptor.h>
#include <threadpool/pool.h>


namespace http {

    class server {

        public:

            server(int buf_len, int max_conn, int max_backlog, int timeout_ms, int thread_num,
                   int listening_port, std::string &listening_ip);
            ~server();

            void start(void); 

        private:

            int default_port = 80;
            std::string default_scheme = "http";
            
            int buf_len;
            int active_conn;
            int max_conn;
            int max_backlog;
            int timeout_ms;
            int thread_num;

            int listening_port;
            std::string listening_ip;

            char *rx_buf;
            std::unique_ptr<threadpool::pool> req_pool;
            sockpp::acceptor acc;

            // need some connection vector that allows us to close, read and write to the underlying socket.

        /* Request handling methods. */
        private:

            void handle_req(sockpp::socket skt, std::string_view raw_req);

            void validate(std::shared_ptr<struct req> req);
            
    };

}
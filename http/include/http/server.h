#pragma once

#include <sockpp/acceptor.h>


namespace http {

    /* Stores configurations for the http server. */
    class server {

        public:

            int max_conn;
            int max_backlog;
            int timeout_ms;

            int thread_num;

            int listening_port;
            std::string listening_ip;

            char recv_buf[10000];

            sockpp::acceptor acc;

            server(int max_conn, int max_backlog, int timeout_ms, int thread_num, int listening_port,
                   std::string &listening_ip)
                : max_conn(max_conn), max_backlog(max_backlog), timeout_ms(timeout_ms), thread_num(thread_num),
                  listening_port(listening_port), listening_ip(listening_ip) { }

            void start(void); 
    };
}
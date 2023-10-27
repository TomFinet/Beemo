#pragma once

#include <sockpp/acceptor.h>
#include <sockpp/socket.h>

#include <threadpool/pool.h>

#include <httpserver/req_worker.h>

namespace httpserver {

const int ip_len = 15;
const int buf_len = 10000;

/* Stores configurations for the http server. */
class server {

    public:

        int max_conn;
        int max_backlog;
        int timeout_ms;

        int thread_num;

        int listening_port;
        char listening_ip[ip_len];

        char recv_buf[buf_len];

        sockpp::acceptor acc;

        server(int max_conn, int max_backlog, int timeout_ms, int thread_num, int listening_port, const char listening_ip[ip_len])
            : max_conn(max_conn), max_backlog(max_backlog), timeout_ms(timeout_ms), thread_num(thread_num),
              listening_port(listening_port), listening_ip(listening_ip) { }

        int start(); 

        void end();
};

}
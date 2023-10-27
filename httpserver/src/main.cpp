#include <httpserver/server.h>

const char ip[15] = "127.0.0.1";
const int port = 9001;
const int thread_num = 4;
const int timeout_ms = 2000;

int main() {
    httpserver::server server(20, 1000, timeout_ms, thread_num, port, ip);
    server.start();
}

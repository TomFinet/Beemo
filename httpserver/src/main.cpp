#include <httpserver/server.h>

const char ip[] = "127.0.0.1";
const int port = 9001;
const int thread_num = 4;

int main() {
    httpserver::server server(20, 1000, thread_num, port, ip);
    server.start();
}

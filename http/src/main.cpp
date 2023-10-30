#include <string>
#include <http/server.h>

namespace {

    std::string ip = "127.0.0.1";
    int port = 9001;
    int thread_num = 4;
    int timeout_ms = 2000;

    int main() {
        http::server server(20, 1000, timeout_ms, thread_num, port, ip);
        server.start();
    }

}
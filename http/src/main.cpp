#include <string>
#include <stdexcept>
#include <iostream>

#include <http/server.h>


std::string ip = "127.0.0.1";
int buf_len = 10000;
int port = 9001;
int thread_num = 4;
int timeout_ms = 2000;

int main()
{
    try {
        http::server server(buf_len, 20, 1000, timeout_ms, thread_num, port, ip);
        server.start();
    }
    catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
}

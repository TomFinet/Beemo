#include <string>
#include <stdexcept>
#include <iostream>

#include <http/server.h>
#include <http/msg.h>
#include <http/routing.h>
#include <http/parser.h>


struct http::config config;

std::string home_handler(struct req *req)
{
    return std::string("X", 1024);
}

int main()
{
    try {
        http::register_req_handler({"/", http::get}, (http::req_handler_t)&home_handler);
        http::server server(config);
        server.start();
    }
    catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
}

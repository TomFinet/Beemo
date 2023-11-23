#include <string>
#include <stdexcept>
#include <iostream>

#include <http/server.h>
#include <http/msg.h>
#include <http/routing.h>


struct http::config config;

void home_handler(struct req *req)
{
    std::cout << "WELCOME HOME BABAY!" << std::endl;
}

int main()
{
    try {
        http::register_req_handler({"/", http::get}, std::function(&home_handler));

        http::server server(config);
        server.start();
    }
    catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
}

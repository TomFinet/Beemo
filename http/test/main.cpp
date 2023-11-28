#include <string>
#include <stdexcept>
#include <iostream>

#include <http/server.h>
#include <http/msg.h>
#include <http/routing.h>


struct http::config config;

std::string home_handler(struct req *req)
{
    return "<html><h1>Tom's Corner of the Web.</h1></html>";
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

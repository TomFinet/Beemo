#include <string>
#include <stdexcept>
#include <iostream>

#include <http/config.h>
#include <http/server.h>
#include <http/msg.h>
#include <http/routing.h>
#include <http/parser.h>

namespace beemo
{
    config cfg;

    std::string home_handler(struct req *req)
    {
        return std::string("X", 1024);
    }
}

int main()
{
    try {
        beemo::register_req_handler({"/", beemo::get}, (beemo::req_handler_t)&beemo::home_handler);
        beemo::server server(beemo::cfg);
        server.start();
    }
    catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}
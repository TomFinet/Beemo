#pragma once

#include <string>

namespace httpserver {

class req_handler {

    public:

        req_handler() {}
        ~req_handler() {}

        void handle(std::string &pkt);
};

}
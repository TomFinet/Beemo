#pragma once

#include <string>
#include <iostream>
#include <thread>

namespace httpserver {

class req_handler {

    public:

        req_handler() {}
        ~req_handler() {}

        void handle(std::string &pkt);
    
    private:

        inline void print_error(const std::exception &ex)
        {
            std::cout << "tid: " << std::this_thread::get_id()
                      << " | " << ex.what() << std::endl;
        }
};

}
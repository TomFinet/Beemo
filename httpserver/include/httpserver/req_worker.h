#pragma once

#include <string>
#include <iostream>
#include <thread>

namespace httpserver {

class req_worker {

    public:

        req_worker() {}
        ~req_worker() {}

        void work(std::string &pkt);

    private:

        void validate(const struct http_req &req);

        inline void print_error(const std::exception &ex)
        {
            std::cout << "tid: " << std::this_thread::get_id()
                      << " | " << ex.what() << std::endl;
        }
};

}
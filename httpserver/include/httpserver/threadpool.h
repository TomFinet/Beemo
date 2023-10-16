#pragma once

#include <functional>

namespace httpserver {

class threadpool {

    public:

        threadpool();
        ~threadpool();

        void submit(std::function<void()> &job);

    private:

        const static int thread_limit = 8;
        
        std::thread threads[thread_limit];

        std::queue<std::function<void()>> jobs;



};

}
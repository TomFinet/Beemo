#pragma once

#include <type_traits>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <stdexcept>
#include <future>
#include <functional>
#include <condition_variable>

namespace beemo
{
    class pool {
        public:
            pool() = delete;
            pool(size_t thread_num, unsigned int timeout_ms);
            ~pool();

            void submit(std::function<void()> job);

        private:
            size_t thread_num;
            unsigned int timeout_ms;

            std::vector<std::thread> workers;
            std::queue<std::function<void()>> jobs;
            std::mutex job_mutex;
            /* used with mutex to block one or more threads until another
            thread both modifies the resource queue) and notifies
            the condition variable. */
            std::condition_variable condition;
            bool stop;
    };

}
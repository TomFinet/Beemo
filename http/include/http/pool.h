#pragma once

#include <type_traits>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <stdexcept>
#include <functional>
#include <condition_variable>

namespace beemo
{
    class pool {
        public:
            pool() = delete;
            pool(size_t thread_num);
            ~pool();

            void submit(std::function<void()> job);

        private:
            size_t thread_num;
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
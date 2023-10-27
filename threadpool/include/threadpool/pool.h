#pragma once

#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <stdexcept>
#include <future>
#include <functional>
#include <condition_variable>

/** taken from https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h */

namespace threadpool {

class pool {

    public:

        /* Creates and starts the threadpool:
            - threads are created
            - threads wait for queued requests
        */
        pool(size_t thread_num, unsigned int timeout_ms);

        /* Waits for all threads to finish and joins them. */
        ~pool();

        template<class F, class... Args>
        void submit(F&& f, Args&&... args) 
        {
            using return_type = typename std::invoke_result<F, Args...>::type;

            auto job = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );
                
            {
                std::unique_lock<std::mutex> lock(job_mutex);

                if(stop)
                    throw std::runtime_error("Trying to submit to a stopped pool...");

                jobs.emplace([job](){ (*job)(); });
            }

            condition.notify_one();
        }

    private:

        size_t thread_num;
        unsigned int timeout_ms;

        std::vector<std::thread> workers;
        std::queue<std::function<void()>> jobs;
        std::mutex job_mutex;
        /* used with mutex to block one or more threads until another
           thread both modifies a shared variable (in this case the
           resource queue) and notifies the condition_variable. */
        std::condition_variable condition;
        bool stop;
};

}
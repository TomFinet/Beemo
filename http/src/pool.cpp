#include <http/pool.h>

namespace beemo
{
    pool::pool(size_t thread_num, unsigned int timeout_ms)
        : stop(false), thread_num(thread_num), timeout_ms(timeout_ms)
    {
        for (int i = 0; i < thread_num; i++) {
            workers.emplace_back([this]
            {
                for (;;) {
                    std::function<void()> job;

                    {
                        std::unique_lock<std::mutex> lock(job_mutex);
                        condition.wait(lock, [this]{ return !this->jobs.empty() || this->stop; });
                        
                        if (stop) {
                            return;
                        }

                        job = jobs.front();
                        jobs.pop();
                    } 

                    /* TODO: call with a timeout */
                    job();
                }
            });
        }
    }

    pool::~pool()
    {
        {
            std::unique_lock<std::mutex> lock(job_mutex);
            stop = true;
        }
        condition.notify_all();
        for (int i = 0; i < thread_num; i++) {
            workers[i].join();
        }
    }

    void pool::submit(std::function<void()> job)
    {
        {
            std::unique_lock<std::mutex> lock(job_mutex);
            if(stop) {
                return;
            }

            jobs.emplace(job);
        }
        condition.notify_one();
    }
}

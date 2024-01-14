#include <threadpool/pool.h>

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

                    {   /* unlock the mutex when exiting scope. */
                        std::unique_lock<std::mutex> lock(job_mutex);
                        condition.wait(lock, [this]{ return !this->jobs.empty() || this->stop; });
                        
                        if (stop) {
                            return;
                        }

                        /* jobs.front() is an rvalue, so do we really need move? */
                        job = std::move(jobs.front());
                        jobs.pop();
                    } 

                    /* call with a timeout */
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

}

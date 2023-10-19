#include <threadpool/pool.h>

namespace threadpool {

pool::pool(size_t thread_num) : stop(false)
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

                    job = std::move(jobs.front());
                    jobs.pop();
                } 

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

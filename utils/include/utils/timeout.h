#pragma once

#include <chrono>
#include <mutex>
#include <future>
#include <memory>


namespace utils
{
    // register timeout event in the event loop
    // if timeout calls while job is still processing
    // how to check if job is currently executing?
    // use an atomic bool named `executing` which we can read
    // from the timeout event, and if it reads true, we send a signal to the
    // processing thread to terminate with a timeout error.
    // advantages, no extra threads needed, we utilise the threadpool
    // problem: epoll does not support timeout events
    // solution: use a separate thread that handles timing events
    template<typename F, typename... Args>
    bool run_with_timeout(const unsigned int timeout_ms, F&& job, Args&&... args)
    {
        std::mutex m;
        std::condition_variable cv;

        auto _ = std::async(std::launch::async, [&cv, &job, &args...]()
        {
            job(std::forward<Args>(args)...);
            cv.notify_one();
        });

        {
            std::unique_lock<std::mutex> lock(m);
            if (cv.wait_for(lock, std::chrono::milliseconds(timeout_ms)) == std::cv_status::timeout) {
                return true;
            }
            return false;
        }
    }

}

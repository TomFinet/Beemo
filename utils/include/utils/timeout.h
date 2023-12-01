#pragma once

#include <utils/timeout_err.h>

#include <chrono>
#include <mutex>
#include <future>
#include <memory>


namespace utils
{

    /* Provides generic timeout function. */
    template<typename F, typename ...Args>
    void run(F&& f, Args&&... args, const unsigned int timeout_sec)
    {
        std::mutex m;
        std::condition_variable cv;

        std::async(std::launch::async, [&cv](F&& f)
        {
            std::forward<F>(f)(std::forward<Args>(args)...);
            cv.notify_one();
        }, f);

        {
            std::unique_lock<std::mutex> lock(m);
            if (cv.wait_for(lock, std::chrono::duration<long long>(timeout_sec)) == std::cv_status::timeout) {
                throw timeout_err();
            }
        }
    }

}
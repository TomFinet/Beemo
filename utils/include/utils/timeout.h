#pragma once

#include <chrono>
#include <mutex>
#include <future>
#include <memory>


namespace utils
{

    template<typename F, typename ...Args>
    bool run(F&& f, const unsigned int timeout_sec, Args&&... args)
    {
        std::mutex m;
        std::condition_variable cv;

        std::async(std::launch::async, [&cv, &f](Args&&... args)
        {
            f(std::forward<Args>(args)...);
            cv.notify_one();
        });

        {
            std::unique_lock<std::mutex> lock(m);
            if (cv.wait_for(lock, std::chrono::duration<long long>(timeout_sec)) == std::cv_status::timeout) {
                return true;
            }
            return false;
        }
    }

}
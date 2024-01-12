#include "epoll_ctx.h"
#include <transport/err.h>
#include <errno.h>

namespace transport
{

    constexpr int block_indefinitely = -1;
    constexpr unsigned int default_events = EPOLLHUP | EPOLLET | EPOLLONESHOT;

    epoll_ctx::epoll_ctx(const int max_events) : max_events_(max_events)
    {
        logger_ = spdlog::stdout_color_mt("epoll_ctx");
        epoll_handle_ = epoll_create1(EPOLL_CLOEXEC);
        if (epoll_handle_ == invalid_handle) {
            throw transport_err();
        } 
    }
    
    epoll_ctx::~epoll_ctx()
    {
        close(epoll_handle_);
    }

    epoll_event* epoll_ctx::get_event(const socket_t skt_handle)
    {
        return events_.at(skt_handle).get();
    }

    void epoll_ctx::add_event(int event_code, socket_t skt_handle)
    {
        if (events_.size() >= max_events_) {
            return;
        }
        
        std::unique_ptr<epoll_event> event = std::make_unique<epoll_event>();
        event->events = default_events | event_code;
        event->data.fd = skt_handle;

        int err = epoll_ctl(epoll_handle_, EPOLL_CTL_ADD, skt_handle, event.get());
        if (err == epoll_error) {
            logger_->info("Failed EPOLL_CTL_ADD, errno: {0}", errno);
            throw transport_err();
        }

        std::unique_lock<std::mutex> lock(events_mutex_);
        events_[skt_handle] = std::move(event);
    }
    
    void epoll_ctx::modify_event(const socket_t skt_handle, unsigned int event_code)
    {
        std::unique_ptr<epoll_event> event = std::make_unique<epoll_event>();
        event->events = default_events | event_code;
        event->data.fd = skt_handle;

        int err = epoll_ctl(epoll_handle_, EPOLL_CTL_MOD, skt_handle, event.get());
        if (err == epoll_error) {
            logger_->info("Failed EPOLL_CTL_ADD, errno: {0}", errno);
            throw transport_err();
        }

        std::unique_lock<std::mutex> lock(events_mutex_);
        events_[skt_handle] = std::move(event);
    }

    int epoll_ctx::wait(epoll_event *ready_events, size_t len)
    {
        return epoll_wait(epoll_handle_, ready_events, len, block_indefinitely);
    }

    void epoll_ctx::remove_event(const socket_t skt_handle)
    {
        std::unique_lock<std::mutex> lock(events_mutex_);
        events_.erase(skt_handle);
    }

    size_t epoll_ctx::size(void)
    {
        std::unique_lock<std::mutex> lock(events_mutex_);
        return events_.size();
    }
    
    io_queue_t epoll_ctx::get_handle(void)
    {
        return epoll_handle_;
    }

}
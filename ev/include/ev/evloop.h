#pragma once

#include <http/conn.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <map>
#include <memory>
#include <mutex>

/* TODO: We could use templates and allow users to register any type T
which implements the necessary callbacks to use the event loop. */
namespace beemo
{
    constexpr int ev_error = -1;

    enum ev_mask {
        EV_IN,
        EV_OUT
    };

    /* Implementation dependent. */
    struct __evloop_ctx;

    struct evloop {

        std::shared_ptr<spdlog::logger> logger_;
        __evloop_ctx* ctx_;

        uint max_evs_;
        std::map<socket_t, std::weak_ptr<conn>> events_;
        std::mutex events_mtx_;

        evloop(uint max_evs);     
        ~evloop();

        int reg(std::shared_ptr<conn> conn, ev_mask mask);
        void loop(uint max_events, int timeout);
        void handle_in(std::shared_ptr<conn> conn);
        void handle_out(std::shared_ptr<conn> conn);

        std::weak_ptr<conn> get_event(socket_t sktfd)
        {
            std::unique_lock<std::mutex> lock(events_mtx_);
            return events_[sktfd];
        }
    };
}
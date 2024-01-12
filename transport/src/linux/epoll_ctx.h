#pragma once

#include <transport/platform.h>
#include <transport/io_ctx.h>
#include <transport/conn_ctx.h>

#include <unordered_map>
#include <mutex>
#include <string_view>
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace transport
{

    class epoll_ctx {
        
        private:
            io_queue_t epoll_handle_;
            int max_events_;
            
            std::unordered_map<socket_t, std::unique_ptr<epoll_event>> events_;
            std::unordered_map<socket_t, std::unique_ptr<io_ctx>> outgoing_io_;
            std::mutex events_mutex_;
            std::mutex outgoing_mutex_;

            std::shared_ptr<spdlog::logger> logger_;

        public:
            epoll_ctx(const int max_events);
            ~epoll_ctx();

            void add_event(int event_code, socket_t skt_handle);
            void modify_event(const socket_t skt_handle, unsigned int event_code);

            void add_io(socket_t skt_handle, std::string_view msg);
            io_ctx* get_io(socket_t skt_handle);

            io_queue_t get_handle(void);
            epoll_event* get_event(const socket_t skt_handle);
            void remove_event(const socket_t skt_handle);

            int wait(epoll_event *event_arr, size_t len);
            size_t size(void);

    };

}
#pragma once

#include <transport/platform.h>
#include <transport/io_buf.h>
#include <transport/err.h>

#include <memory>


namespace beemo
{
    struct socket {

        socket_t handle_;
        int last_error_; 

        socket() : handle_(invalid_handle), last_error_(0) {}
        socket(socket_t handle) : handle_(handle), last_error_(0) {}
        ~socket();

        static void startup(void)
        {
            #ifdef _WIN32
            WSADATA wsaData;
            int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (err != 0) {
                throw transport_err();
            }
            #endif
        }

        static void cleanup(void)
        {
            #ifdef _WIN32
            WSACleanup();
            #endif
        } 

        void create_handle(int family, int type, int protocol);
        void bind(const sockaddr* addr, uint nbytes);
        void listen(uint backlog);
        socket_t accept(void);

        int rx(io_buf *const io, const int buf_num);
        int tx(io_buf *const io, const int buf_num);

        int close_rx(void);
        int close_tx(void);
        int close_rtx(void);
        
        bool would_block(void);
        void blocking(bool set);
        void set_options(int level, int optname, const char* optval, int oplen);
        void set_error(void);
        void address(sockaddr* name, socklen_t* namelen);
    };
}
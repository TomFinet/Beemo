#pragma once

#include <stdexcept>
#include <memory>

#include <transport/platform.h>
#include <transport/io_ctx.h>
#include <transport/err.h>


namespace transport {

    class socket {

    private:

        socket_t handle_;
        int last_error_; 

    public:

        socket() : handle_(invalid_handle), last_error_(0) {}
        socket(socket_t handle) : handle_(handle), last_error_(0) {}
        
        ~socket()
        {
            close();
        }

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

        void bind(const struct sockaddr *addr, unsigned int nbytes)
        {
            int err = ::bind(handle_, addr, nbytes);
            if (err == socket_error) {
                close();
            }
        }

        void listen(unsigned int backlog)
        {
            int err = ::listen(handle_, backlog);
            if (err == socket_error) {
                close();
            }
        }
        
        socket_t accept(void);

        void set_options(int level, int optname, const char* optval, int oplen)
        {
            int err = setsockopt(handle_, level, optname, optval, oplen);
            if (err == socket_error) {
                close();
            }
        }

        void rx(io_ctx *const io, const int buf_num);
        void tx(io_ctx *const io, const int buf_num);

        socket_t handle(void)
        {
            return handle_;
        }

        void set_handle(socket_t handle)
        {
            handle_ = handle;
        }

        void address(struct sockaddr *name, socklen_t *namelen)
        {
            int err = getsockname(handle_, name, namelen);
            if (err == socket_error) {
                close();
            }
        }

        int get_last_error(void);

        void close_tx(void);
        void close(void);
    };
}
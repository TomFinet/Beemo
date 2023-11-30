#pragma once

#include <stdexcept>
#include <memory>

#include <transport/platform.h>
#include <transport/io_ctx.h>


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
                throw std::runtime_error("Failed to initialise the Winsock dll. Quitting...");
            }
            #endif
        }

        static void cleanup(void)
        {
            #ifdef _WIN32
            WSACleanup();
            #endif
        } 

        /**
        * Creates a handle for this socket.
        * 
        * @param family The socket address family (unspecified, IPv4, IPv6, NetBIOS, ...).
        * @param type The socket type (stream, datagram, raw, ...).
        * @param protocol The socket protocol (TCP, UDP, ...).
        */
        void create_handle(int family, int type, int protocol);

        /**
        * Binds a local address to this socket.
        *
        * @param addr Pointer to local address.
        * @param nbytes Size in bytes of local address structure.
        *
        */
        void bind(const struct sockaddr *addr, unsigned int nbytes)
        {
            int err = ::bind(handle_, addr, nbytes);
            if (err == socket_error) {
                throw std::runtime_error("Failed to bind an address to the socket handle.");
            }
        }

        /**
        * Changes socket state to listening on the bound address. 
        */
        void listen(unsigned int backlog)
        {
            int err = ::listen(handle_, backlog);
            if (err == socket_error) {
                throw std::runtime_error("Failed to transition socket into a listening state.");
            }
        }
        
        socket_t accept(void);

        void set_options(int level, int optname, const char* optval, int oplen)
        {
            int err = setsockopt(handle_, level, optname, optval, oplen);
            if (err == socket_error) {
                throw std::runtime_error("Failed to set socket options.");
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
                throw std::runtime_error("Failed to get the socket address.");
            }
        }

        int get_last_error(void);

        void close_tx(void);
        void close(void);
    };
}
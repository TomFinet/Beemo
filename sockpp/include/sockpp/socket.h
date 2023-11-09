#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdexcept>
#include <memory>

#include <sockpp/io_ctx.h>


namespace sockpp {

    using socket_t = SOCKET;

    class socket {

    private:

        socket_t handle_;
        int last_error_; 

    public:

        socket() : handle_(INVALID_SOCKET), last_error_(0) {}
        socket(socket_t handle) : handle_(handle), last_error_(0) {}
        ~socket();

        static void startup(void)
        {
            WSADATA wsaData;
            int err = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (err != 0) {
                throw std::runtime_error("Failed to initialise the Winsock dll. Quitting...");
            }
        }

        static void cleanup(void)
        {
            ::WSACleanup();
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
        void bind(const struct sockaddr *addr, int nbytes);

        /**
        * Changes socket state to listening on the bound address. 
        */
        void listen(unsigned int backlog);

        void set_options(int level, int optname, const char *optval, int oplen);

        /**
        * Blocking call.
        */
        socket_t accept(void);

        void rx(io_ctx *const io, const int buf_num);
        void tx(io_ctx *const io, const int buf_num);

        void close(void);

        socket_t handle(void);

        void address(struct sockaddr *name, int *namelen);

        int get_last_error(void);
    };
}
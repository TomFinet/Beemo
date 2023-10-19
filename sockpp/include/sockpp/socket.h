#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

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

    static int startup(void)
    {
        WSADATA wsaData;
        return ::WSAStartup(MAKEWORD(2, 2), &wsaData);
    }

    static int cleanup(void)
    {
        return ::WSACleanup();
    } 

    /**
     * Creates a handle for this socket.
     * 
     * @param family The socket address family (unspecified, IPv4, IPv6, NetBIOS, ...).
     * @param type The socket type (stream, datagram, raw, ...).
     * @param protocol The socket protocol (TCP, UDP, ...).
     * 
     * @return Handle referencing this socket or INVALID_SOCKET on failure.
     */
    socket_t create_handle(int family, int type, int protocol);

    /**
     * Binds a local address to this socket.
     *
     * @param addr Pointer to local address.
     * @param nbytes Size in bytes of local address structure.
     *
     * @return Error code where zero indicates success, otherwise SOCKET_ERROR.
     */
    int bind(struct sockaddr *addr, int nbytes);

    /**
     * Changes socket state to listening on the bound address. 
     *
     * @return Error code.
     */
    int listen(void);

    /**
     * Blocking call.
     */
    socket_t accept(void);

    int receive(char *const buf, int len, int flags);

    socket_t handle(void);

    int address(struct sockaddr *name, int *namelen);
};

}
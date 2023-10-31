#include <sockpp/socket.h>


namespace sockpp {

    socket::~socket()
    {
        if (handle_ != INVALID_SOCKET) {
            closesocket(handle_);
        }
    }

    void socket::create_handle(int family, int type, int protocol)
    {
        handle_ = ::socket(family, type, protocol); 
        if (handle_ == INVALID_SOCKET) {
            throw std::runtime_error("Failed to create the socket.");
        }
    }

    void socket::bind(struct sockaddr *addr, int nbytes)
    {
        int err = ::bind(handle_, addr, nbytes);
        if (err == SOCKET_ERROR) {
            throw std::runtime_error("Failed to bind an address to the socket handle.");
        }
    }

    void socket::listen(void)
    {
        int err = ::listen(handle_, SOMAXCONN);
        if (err == SOCKET_ERROR) {
            throw std::runtime_error("Failed to transition socket into a listening state.");
        }
    }

    socket_t socket::accept(void)
    {
        socket_t conn_handle = ::accept(handle_, NULL, NULL);
        if (conn_handle == INVALID_SOCKET) {
            throw std::runtime_error("Failed to accept an incoming connection.");
        }
        return conn_handle;
    }

    int socket::receive(char *const buf, int len, int flags)
    {
        int nbytes = ::recv(handle_, buf, len, flags);
        if (nbytes == SOCKET_ERROR) {
            throw std::runtime_error("Failed to receive the incoming data.");
        }
        return nbytes;
    }

    socket_t socket::handle(void)
    {
        return handle_;
    }

    void socket::address(struct sockaddr *name, int *namelen)
    {
        int err = ::getsockname(handle_, name, namelen);
        if (err == SOCKET_ERROR) {
            throw std::runtime_error("Failed to get the socket address.");
        }
    }

}
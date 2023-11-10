#include <sockpp/socket.h>
#include <iostream>


namespace sockpp {

    socket::~socket()
    {
        std::cout << "~socket()" << std::endl;
        if (handle_ != INVALID_SOCKET) {
            closesocket(handle_);
        }
    }

    void socket::create_handle(int family, int type, int protocol)
    {
        handle_ = ::WSASocketW(family, type, protocol, nullptr, 0, WSA_FLAG_OVERLAPPED); 
        if (handle_ == INVALID_SOCKET) {
            throw std::runtime_error("Failed to create the socket.");
        }
    }

    void socket::bind(const struct sockaddr *addr, int nbytes)
    {
        int err = ::bind(handle_, addr, nbytes);
        if (err == SOCKET_ERROR) {
            throw std::runtime_error("Failed to bind an address to the socket handle.");
        }
    }

    void socket::listen(unsigned int backlog)
    {
        int err = ::listen(handle_, backlog);
        if (err == SOCKET_ERROR) {
            throw std::runtime_error("Failed to transition socket into a listening state.");
        }
    }
    
    void socket::set_options(int level, int optname, const char *optval, int oplen)
    {
        int err = ::setsockopt(handle_, level, optname, optval, oplen);
        if (err == SOCKET_ERROR) {
            throw std::runtime_error("Failed to set socket options.");
        }
    }

    socket_t socket::accept(void)
    {
        /* extract first connection on the queue of pending connections for this socket. */
        socket_t conn_handle = ::WSAAccept(handle_, nullptr, nullptr, nullptr, 0);
        if (conn_handle == INVALID_SOCKET) {
            throw std::runtime_error("Failed to accept an incoming connection.");
        }
        return conn_handle;
    }

    void socket::rx(io_ctx *const io, const int buf_num)
    {
        unsigned long nbytes = 0;
        unsigned long flags = 0;
        int err = ::WSARecv(handle_, &io->buf_desc, buf_num, &nbytes, &flags, (OVERLAPPED*)io, nullptr);
        if (err == SOCKET_ERROR && (ERROR_IO_PENDING != get_last_error())) {
            delete io;
            throw std::runtime_error("Failed to receive the incoming data.");
        }
    }

    void socket::tx(io_ctx *const io, const int buf_num)
    {
        DWORD nbytes = 0;
        int err = ::WSASend(handle_, &io->buf_desc, buf_num, &nbytes, 0,
                            (OVERLAPPED*)io, nullptr);
        if (err == SOCKET_ERROR && (ERROR_IO_PENDING != get_last_error())) {
            std::cout << "tx error: " << last_error_ << std::endl;
            delete io;
            throw std::runtime_error("Failed to send the bytes in the buffer.");
        }
    }

    void socket::close(void)
    {
        ::closesocket(handle_);
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

    int socket::get_last_error(void)
    {
        last_error_ = WSAGetLastError();
        return last_error_;
    }

}
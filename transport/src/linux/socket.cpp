#include <transport/socket.h>

#include <unistd.h>
#include <fcntl.h>
#include <iostream>


namespace transport
{

    void socket::create_handle(int family, int type, int protocol)
    {
        handle_ = ::socket(family, type, protocol); 
        if (handle_ == invalid_handle) {
            last_error_ = errno;
            throw transport_err();
        }   
    }

    socket_t socket::accept(void)
    {
        socket_t conn_handle = ::accept(handle_, NULL, NULL);
        if (conn_handle == invalid_handle) {
            last_error_ = errno; 
            throw transport_err();
        }
        return conn_handle;
    }

    void socket::blocking(bool block)
    {
        int flags = fcntl(handle_, F_GETFL);
        if (flags == socket_error) {
            last_error_ = errno;
            throw transport_err();
        }

        if (block) {
            flags &= ~O_NONBLOCK;
        }
        else {
            flags |= O_NONBLOCK;
        }
        
        if (fcntl(handle_, F_SETFL, flags) == socket_error) {
            last_error_ = errno;
            throw transport_err();
        }
    }

    int socket::rx(io_ctx *const io, const int buf_num)
    {
        int nbytes = recv(handle_, io->buf + io->bytes_rx, io->buf_len_ - io->bytes_rx, 0);
        if (nbytes == socket_error) {
            last_error_ = errno;
        }
        return nbytes;
    }

    int socket::tx(io_ctx *const io, const int buf_num)
    {
        int nbytes = send(handle_, io->buf + io->bytes_tx, io->bytes_to_tx - io->bytes_tx, MSG_NOSIGNAL);
        if (nbytes == socket_error) {
            last_error_ = errno;
        }
        return nbytes;
    }

    int socket::get_last_error(void)
    {
        return last_error_;
    }

    void socket::close_tx(void)
    {
        if (handle_ != invalid_handle) {
            shutdown(handle_, SHUT_WR);
        }
    }

    void socket::close(void)
    {
        if (handle_ != invalid_handle) {
            ::close(handle_);
        }
    }
   
}

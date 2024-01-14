#include <transport/socket.h>

#include <fcntl.h>
#include <unistd.h>


namespace beemo
{
    socket::~socket()
    {
        close_rtx();
        close(handle_);
    }

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

    void socket::blocking(bool set)
    {
        int flags = fcntl(handle_, F_GETFL);
        if (flags == socket_error) {
            last_error_ = errno;
            throw transport_err();
        }

        if (set) {
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

    bool socket::would_block(void)
    {
        return last_error_ == EAGAIN || last_error_ == EWOULDBLOCK;
    }

    int socket::rx(io_buf *const io, const int buf_num)
    {
        int nbytes = recv(handle_, io->buf + io->bytes_rx, io->buf_len_ - io->bytes_rx, 0);
        if (nbytes == socket_error) {
            last_error_ = errno;
        }
        return nbytes;
    }

    int socket::tx(io_buf *const io, const int buf_num)
    {
        int nbytes = send(handle_, io->buf + io->bytes_tx, io->bytes_to_tx - io->bytes_tx, MSG_NOSIGNAL);
        if (nbytes == socket_error) {
            last_error_ = errno;
        }
        return nbytes;
    }

    int socket::close_rx(void)
    {
        int err = shutdown(handle_, SHUT_RD);
        if (err == invalid_handle) {
            last_error_ = errno;
        }
        return err;
    }

    int socket::close_tx(void)
    {
        int err = shutdown(handle_, SHUT_WR);
        if (err == invalid_handle) {
            last_error_ = errno;
        }
        return err;
    }

    int socket::close_rtx(void)
    {
        int err = shutdown(handle_, SHUT_RDWR);
        if (err == invalid_handle) {
            last_error_ = errno;
        }
        return err;
    }

    void socket::set_error(void)
    {
        last_error_ = errno;
    }
}
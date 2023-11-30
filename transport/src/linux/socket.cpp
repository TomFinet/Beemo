#include <sockpp/socket.h>


namespace sockpp
{

    void socket::create_handle(int family, int type, int protocol)
    {
        handle_ = ::socket(family, type, protocol); 
        if (handle_ == invalid_handle) {
            throw std::runtime_error("Failed to create the socket.");
        }   
    }

    socket_t socket::accept(void)
    {
        socket_t conn_handle = ::accept(handle_, NULL, NULL);
        if (conn_handle == invalid_handle) {
            throw std::runtime_error("Failed to accept an incoming connection.");
        }
        return conn_handle;
    }

    void socket::rx(io_ctx *const io, const int buf_num)
    {
        int nbytes = recv(handle_, io->buf, sizeof(io->buf), 0);
        if (nbytes == socket_error) {
            // something
        }
        io->bytes_rx = nbytes;
    }

    void socket::tx(io_ctx *const io, const int buf_num)
    {
        int nbytes = send(handle_, io->buf, sizeof(io->bytes_to_tx));
        /* TODO: finish the method. */
    }

    int socket::get_last_error(void)
    {
        last_error_ = errno;
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
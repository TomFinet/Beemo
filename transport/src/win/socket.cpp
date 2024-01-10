#include <transport/socket.h>


namespace transport {

    void socket::create_handle(int family, int type, int protocol)
    {
        handle_ = WSASocketW(family, type, protocol, nullptr, 0, WSA_FLAG_OVERLAPPED); 
        if (handle_ == invalid_handle) {
            throw transport_err();
        }
    }

    socket_t socket::accept(void)
    {
        socket_t conn_handle = WSAAccept(handle_, nullptr, nullptr, nullptr, 0);
        if (conn_handle == invalid_handle) {
            get_last_error();
            throw transport_err();
        }
        return conn_handle;
    }

    void socket::non_blocking(void) { }

    void socket::rx(io_ctx *const io, const int buf_num)
    {
        unsigned long nbytes = 0;
        unsigned long flags = 0;
        int err = WSARecv(handle_, &io->buf_desc, buf_num, &nbytes, &flags, (OVERLAPPED*)io, nullptr);
        if (err == socket_error && (ERROR_IO_PENDING != get_last_error())) {
            delete io;
            close();
        }
    }

    void socket::tx(io_ctx *const io, const int buf_num)
    {
        DWORD nbytes = 0;
        int err = WSASend(handle_, &io->buf_desc, buf_num, &nbytes, 0,
                            (OVERLAPPED*)io, nullptr);
        if (err == socket_error && (ERROR_IO_PENDING != get_last_error())) {
            delete io;
            close();
        }
    }

    int socket::get_last_error(void)
    {
        last_error_ = WSAGetLastError();
        return last_error_;
    }

    void socket::close_tx(void)
    {
        if (handle_ == invalid_handle) {
            return;
        }

        int err = shutdown(handle_, SD_SEND);
        if (err == socket_error) {
            close();
        }
    }

    void socket::close(void)
    {
        if (handle_ != invalid_handle) {
            closesocket(handle_);
        }
    }

}
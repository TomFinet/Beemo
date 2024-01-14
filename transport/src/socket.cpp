#include <transport/socket.h>


namespace beemo
{
    void socket::bind(const sockaddr* addr, uint nbytes)
    {
        if (::bind(handle_, addr, nbytes) == socket_error) {
            set_error();
            throw transport_err();
        }
    }
    
    void socket::listen(uint backlog)
    {
        if (::listen(handle_, backlog) == socket_error) {
            throw transport_err();
        }
    }

    void socket::set_options(int level, int optname, const char* optval, int oplen)
    {
        if (setsockopt(handle_, level, optname, optval, oplen) == socket_error) {
            set_error();
            throw transport_err();
        }
    }
    
    void socket::address(sockaddr* name, socklen_t* namelen)
    {
        if (getsockname(handle_, name, namelen) == socket_error) {
            set_error();
        }
    }
}
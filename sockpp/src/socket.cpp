#include <sockpp/socket.h>

namespace sockpp {

socket::~socket()
{
    if (handle_ != INVALID_SOCKET) {
        closesocket(handle_);
    }
}

socket_t socket::create_handle(int family, int type, int protocol)
{
    handle_ = ::socket(family, type, protocol); 
    return handle_;
}

int socket::bind(struct sockaddr *addr, int nbytes)
{
    return ::bind(handle_, addr, nbytes);
}

int socket::listen(void)
{
    return ::listen(handle_, SOMAXCONN);
}

socket_t socket::accept(void)
{
    return ::accept(handle_, NULL, NULL);
}

int socket::receive(char *const buf, int len, int flags)
{
    return ::recv(handle_, buf, len, flags);
}

socket_t socket::handle(void)
{
    return handle_;
}

int socket::address(struct sockaddr *name, int *namelen)
{
    return ::getsockname(handle_, name, namelen);
}

}
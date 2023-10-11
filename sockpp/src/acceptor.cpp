#include "acceptor.h"

namespace sockpp {

int acceptor::open(struct sockaddr_in *addr)
{
    int err = sock_.create_handle(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (err == INVALID_SOCKET) {
        return err;
    }

    err = sock_.bind(reinterpret_cast<struct sockaddr*>(addr), sizeof(struct sockaddr_in));
    if (err != 0) {
        return err;
    }

    err = sock_.listen();
    if (err == SOCKET_ERROR) {
        return err;
    }

    return 0;
}

socket_t acceptor::accept(void)
{
    return sock_.accept();
}

}
#include <sockpp/acceptor.h>

namespace sockpp {

int acceptor::open(const std::string ip, const int port)
{
    int err = static_cast<int>(sock_.create_handle(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    if (err == INVALID_SOCKET) {
        return err;
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &local.sin_addr.s_addr);
    local.sin_port = htons(port);

    err = sock_.bind(reinterpret_cast<struct sockaddr*>(&local), sizeof(local));
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
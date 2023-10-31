#include <sockpp/acceptor.h>

namespace sockpp {

    void acceptor::open(const std::string ip, const int port)
    {
        sock_.create_handle(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        struct sockaddr_in local;
        local.sin_family = AF_INET;
        inet_pton(AF_INET, ip.c_str(), &local.sin_addr.s_addr);
        local.sin_port = htons(port);

        sock_.bind(reinterpret_cast<struct sockaddr*>(&local), sizeof(local));
        sock_.listen();
    }

    socket_t acceptor::accept(void)
    {
        return sock_.accept();
    }

}
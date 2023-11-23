#include <sockpp/acceptor.h>


namespace sockpp
{

    void acceptor::open(const std::string &ip, const int port, const unsigned int backlog)
    {
        sock_.create_handle(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        struct sockaddr_in local;
        local.sin_family = AF_INET;
        inet_pton(AF_INET, ip.c_str(), &local.sin_addr.s_addr);
        local.sin_port = htons(port);

        sock_.bind(reinterpret_cast<struct sockaddr*>(&local), sizeof(local));
        /* TODO: what does backlog mean here? Referring to connections or requests? */
        sock_.listen(backlog);

        int zero = 0;
        sock_.set_options(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&zero), sizeof(zero));
    }

}
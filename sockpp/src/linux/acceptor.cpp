#include <sockpp/acceptor.h>


namespace sockpp
{

    void acceptor::open(const std::string &ip, const int port, const unsigned int backlog)
    {
        sock_.create_handle(AF_INET, SOCK_STREAM, 0);

        int one = 1;
        sock_.set_options(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>(&one), sizeof(one));

        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons(port);

        sock_.bind(reinterpret_cast<struct sockaddr*>(&local), sizeof(local));
        sock_.listen(backlog);
    }

}
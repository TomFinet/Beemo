#include <transport/acceptor.h>


namespace beemo
{
    void acceptor::open(const std::string &ip, const int port, const unsigned int backlog,
        const unsigned short linger_sec, const unsigned int rx_buf_len,
        const unsigned int rx_idle_timeout)
    {
        sock_.create_handle(AF_INET, SOCK_STREAM, 0);

        int set = 1;
        /* TODO: what does this line do? */
        //sock_.set_options(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>(&set), sizeof(set));
        sock_.set_options(SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&set), sizeof(set));

        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons(port);

        sock_.bind(reinterpret_cast<struct sockaddr*>(&local), sizeof(local));
        sock_.listen(backlog);
    }
}
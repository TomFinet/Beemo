#include <transport/acceptor.h>


namespace transport
{

    void acceptor::open(const std::string &ip, const int port, const unsigned int backlog,
                        const unsigned short linger_sec, const unsigned int rx_buf_len)
    {
        sock_.create_handle(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        struct sockaddr_in local;
        local.sin_family = AF_INET;
        inet_pton(AF_INET, ip.c_str(), &local.sin_addr.s_addr);
        local.sin_port = htons(port);

        sock_.bind(reinterpret_cast<struct sockaddr*>(&local), sizeof(local));
        sock_.listen(backlog);

        const int zero = 0;
        const int one = 1;
        struct linger l = {1, linger_sec};

        sock_.set_options(SOL_SOCKET, SO_SNDBUF, (char *)&zero, sizeof(zero));
        sock_.set_options(SOL_SOCKET, SO_LINGER, (char *)&l, sizeof(l));
        sock_.set_options(SOL_SOCKET, SO_KEEPALIVE, (char *)&one, sizeof(one)); /* not sure if needed. */
        sock_.set_options(SOL_SOCKET, SO_RCVBUF, (char *)&rx_buf_len, sizeof(rx_buf_len));
    }

}
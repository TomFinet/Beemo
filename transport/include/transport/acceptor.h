#pragma once

#include <transport/socket.h>

#include <string>


namespace beemo
{
    struct acceptor {

            socket sock_;

            acceptor() { }
            ~acceptor() { }

            /**
             * Creates the acceptor's socket handle, binds @addr to it, and
             * changes the socket's state to listening.
             */
            void open(const std::string &ip, const int port, const unsigned int backlog,
                      const unsigned short linger_sec, const unsigned int rx_buf_len,
                      const unsigned int rx_idle_timeout);

            socket_t accept()
            {
                return sock_.accept();
            }
    };
}
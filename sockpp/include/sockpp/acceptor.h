#pragma once

#include <string>

#include <sockpp/socket.h>

namespace sockpp
{

    class acceptor {

        private:

            socket sock_;

        public:

            acceptor() { }
            ~acceptor() { }

            /**
             * Creates the acceptor's socket handle, binds @addr to it, and
             * changes the socket's state to listening.
             */
            void open(const std::string &ip, const int port, const unsigned int backlog,
                      const unsigned short linger_sec, const unsigned int rx_buf_len);

            /**
             * Accepts incoming connections.
             */
            socket_t accept()
            {
                return sock_.accept();
            }
    };

}
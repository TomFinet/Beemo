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
            void open(const std::string &ip, const int host, const unsigned int backlog);

            /**
             * Accepts incoming connections.
             */
            socket_t accept()
            {
                return sock_.accept();
            }
    };

}
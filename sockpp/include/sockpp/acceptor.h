#pragma once

#include <string>

#include <sockpp/socket.h>

namespace sockpp {

/**
 * Used by servers to accept connections.
 */
class acceptor {

    private:

        socket sock_;

    public:

        /**
         * Creates the acceptor's socket handle, binds @addr to it, and
         * changes the socket's state to listening.
         */
        void open(const std::string &ip, const int host, const unsigned int backlog);

        /**
         * Accepts incoming connections.
         */
        socket_t accept(); 
};

}
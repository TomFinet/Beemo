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
        int open(const std::string ip, const int host);

        /**
         * Accepts incoming connections for an open socket.
         */
        socket_t accept(); 
};

}
#pragma once

#include "socket.h"

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
         *
         * @param addr Pointer to the local address to bind to.
         */
        int open(struct sockaddr_in *addr);

        /**
         * Accepts incoming connections for an open socket.
         */
        socket_t accept(); 
};

}
#pragma once

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#define INVALID_HANDLE INVALID_SOCKET

#elif __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <errno.h>
#endif

namespace sockpp
{

#ifdef WIN32
    using socket_t = SOCKET;

    constexpr socket_t invalid_handle = INVALID_SOCKET;    
    constexpr int socket_error = SOCKET_ERROR;
    
#elif __linux__
    using socket_t = int;

    constexpr int invalid_handle = -1;
    constexpr int socket_error = -1;
    
#endif

}
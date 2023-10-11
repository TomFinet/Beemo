#include "socket.h"
#include "acceptor.h"

#include <iostream>

const int BUF_SIZE = 500;

int main() {
    std::cout << "Starting TCP socket server..." << std::endl;

    if (sockpp::socket::startup() != 0) {
        std::cout << "Failed to initialise the Winsock dll. Quitting..." << std::endl;
        return 1;
    }

    sockpp::acceptor acc;

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &local.sin_addr.s_addr);
    local.sin_port = htons(9843);

    if (acc.open(&local) != 0) {
        std::cout << "Failed to open the acceptor..." << std::endl;
        return 1;
    }

    char buf[BUF_SIZE];

    while (true) {
        sockpp::socket_t client_sock = acc.accept();
        if (client_sock == INVALID_SOCKET) {
            std::cout << "Failed to accept connection, trying again." << std::endl;
            continue;
        }

        int res = recv(client_sock, buf, BUF_SIZE, 0);
        if (res == 0) {
            std::cout << "Closing the connection..." << std::endl;
            break;
        } else if (res < 0) {
            std::cout << "Failed to receive buffer. Quitting..." << std::endl;
            sockpp::socket::cleanup();
            return 1;
        }

        for (int i = 0; i < res; i++) {
            std::cout << buf[i];
        }
        std::cout << std::endl;
    }
}
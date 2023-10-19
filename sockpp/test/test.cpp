#include <sockpp/socket.h>
#include <sockpp/acceptor.h>

#include <iostream>

const int BUF_SIZE = 500;

int main() {
    std::cout << "Starting TCP socket server..." << std::endl;

    if (sockpp::socket::startup() != 0) {
        std::cout << "Failed to initialise the Winsock dll. Quitting..." << std::endl;
        return 1;
    }

    sockpp::acceptor acc;

    if (acc.open("127.0.0.1", 9483) != 0) {
        std::cout << "Failed to open the acceptor..." << std::endl;
        return 1;
    }

    char buf[BUF_SIZE];

    while (true) {
        sockpp::socket_t client_handle = acc.accept();
        sockpp::socket client_sock(client_handle);

        if (client_handle == INVALID_SOCKET) {
            std::cout << "Failed to accept connection, trying again." << std::endl;
            continue;
        }

        int res = (client_sock, buf, BUF_SIZE, 0);
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
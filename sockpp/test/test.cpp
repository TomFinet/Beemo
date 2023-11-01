#include <sockpp/socket.h>
#include <sockpp/acceptor.h>

#include <iostream>
#include <stdexcept>

const int BUF_SIZE = 500;

int main() {
    try {
        std::cout << "Starting TCP socket server..." << std::endl;

        sockpp::socket::startup();

        sockpp::acceptor acc;
        acc.open("127.0.0.1", 9483);

        char buf[BUF_SIZE];

        while (true) {
            sockpp::socket_t client_handle = acc.accept();
            sockpp::socket client_sock(client_handle);

            int res = client_sock.rx(buf, BUF_SIZE, 0);
            if (res == 0) {
                continue;
            }

            for (int i = 0; i < res; i++) {
                std::cout << buf[i];
            }
            std::cout << std::endl;
        }
    }
    catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
        sockpp::socket::cleanup();
    }
}
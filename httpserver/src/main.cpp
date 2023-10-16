#include <sockpp/socket.h>
#include <sockpp/acceptor.h>

#include <httpserver/threadpool.h>
#include <httpserver/req_handler.h>

const int ethernet_mtu = 1500;

int main() {

    if (sockpp::socket::startup() != 0) {
        return 1;
    }

    sockpp::acceptor acc;

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &local.sin_addr.s_addr);
    local.sin_port = htons(9843);

    if (acc.open(&local) != 0) {
        return 1;
    }

    httpserver::threadpool threadpool;

    char buf[ethernet_mtu];

    while (true) {
        sockpp::socket_t client_sock = acc.accept();
        if (client_sock == INVALID_SOCKET) {
            continue;
        }

        int res = recv(client_sock, buf, ethernet_mtu, 0);
        if (res < 0) {
            sockpp::socket::cleanup();
            return 1;
        }

        threadpool.submit();

        // TODO: dispatch packet to thread for handling 

    }
}

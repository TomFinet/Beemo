#include <string>

#include <sockpp/socket.h>
#include <sockpp/acceptor.h>

#include <threadpool/pool.h>
#include <httpserver/req_handler.h>

const int ethernet_mtu = 1500;
const char ip[] = "127.0.0.1";
const int port = 9001;
const int thread_num = 4;

int main() {

    if (sockpp::socket::startup() != 0) {
        return 1;
    }

    sockpp::acceptor acc;

    if (acc.open(ip, port) != 0) {
        return 1;
    }

    threadpool::pool threadpool(thread_num);

    char buf[ethernet_mtu];

    while (true) {
        sockpp::socket_t client_handle = acc.accept();
        sockpp::socket client_sock(client_handle);

        if (client_handle == INVALID_SOCKET) {
            continue;
        }

        int res = client_sock.receive(buf, ethernet_mtu, 0);
        if (res < 0) {
            sockpp::socket::cleanup();
            return 1;
        }

        std::string pkt(buf, res);

        threadpool.submit([](std::string pkt)
        {
            httpserver::req_handler handler;
            handler.handle(pkt);
        }, pkt);
    }
}

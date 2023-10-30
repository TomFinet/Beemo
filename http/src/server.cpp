#include <http/server.h>
#include <http/req_worker.h>

#include <sockpp/socket.h>

#include <threadpool/pool.h>

namespace http {

    int server::start(void) {

        if (sockpp::socket::startup() != 0) {
            return 1;
        }

        if (acc.open(listening_ip, listening_port) != 0) {
            return 1;
        }

        threadpool::pool threadpool(thread_num, timeout_ms);

        while (true) {
            sockpp::socket_t client_handle = acc.accept();
            sockpp::socket client_sock(client_handle);

            if (client_handle == INVALID_SOCKET) {
                continue;
            }

            int res = client_sock.receive(recv_buf, 10000, 0);
            if (res < 0) {
                sockpp::socket::cleanup();
                return 1;
            }

            std::string pkt(recv_buf, res);

            threadpool.submit([](std::string pkt)
            {
                req_worker worker;
                worker.work(pkt);
            }, pkt);
        }
    }

}
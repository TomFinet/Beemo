#include <http/server.h>
#include <http/req_worker.h>

#include <sockpp/socket.h>

#include <threadpool/pool.h>

namespace http {

    void server::start(void) {

        sockpp::socket::startup();
        acc.open(listening_ip, listening_port);

        threadpool::pool threadpool(thread_num, timeout_ms);

        while (true) {
            sockpp::socket_t client_handle = acc.accept();
            sockpp::socket client_sock(client_handle);

            int res = client_sock.receive(recv_buf, 10000, 0);
            if (res < 0) {
                sockpp::socket::cleanup();
                return;
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
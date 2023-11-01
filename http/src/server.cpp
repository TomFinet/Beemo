#include <http/server.h>
#include <http/http_parser.h>

#include <sockpp/socket.h>


namespace http
{

    server::server(int buf_len, int max_conn, int max_backlog, int timeout_ms,
                   int thread_num, int listening_port, std::string &listening_ip)
        : active_conn(0), buf_len(buf_len), max_conn(max_conn), max_backlog(max_backlog), timeout_ms(timeout_ms),
          thread_num(thread_num), listening_port(listening_port), listening_ip(listening_ip)
    {
        sockpp::socket::startup();
        rx_buf = new char[buf_len];
        req_pool = std::make_unique<threadpool::pool>(thread_num, timeout_ms);
    }

    server::~server()
    {
        delete[] rx_buf;
        sockpp::socket::cleanup();
    }

    void server::start(void)
    {
        acc.open(listening_ip, listening_port);

        while (true) {
            int res;
            do { 
                sockpp::socket client_sock(acc.accept());

                res = client_sock.rx(rx_buf, buf_len, 0);

                std::string raw_req(rx_buf, res);
                req_pool->submit([this](sockpp::socket skt, std::string_view raw_req)
                {
                    this->handle_req(skt, raw_req);
                }, client_sock, raw_req);
            } while (res > 0);
        }
    }

    void server::handle_req(sockpp::socket skt, std::string_view raw_req)
    {
        std::shared_ptr<struct req> req = parse_headers(raw_req);
        req->print();
        char ok[9] = "200 OK\r\n";
        skt.tx(static_cast<char *>(ok), 9, 0);
    }
    
    void server::validate(std::shared_ptr<struct req> req)
    {
        if (req->version.major != 1 || req->version.minor != 1) {
            throw std::domain_error("Server only supports HTTP 1.1");
        }

        if (req->fields[host_header].empty()) {
            throw std::domain_error("Host header field cannot be missing.");
        }

        if (!req->uri.scheme.empty() && req->uri.scheme != default_scheme) {
            throw std::domain_error("Unsupported scheme.");
        }

        if (req->uri.userinfo.empty()) {
            throw std::domain_error("HTTP has depricated userinfo.");
        }
    }

}

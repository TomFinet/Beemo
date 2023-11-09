#include <http/server.h>
#include <http/http_parser.h>

#include <sockpp/socket.h>
#include <sockpp/io_ctx.h>

#include <iostream>

namespace http
{

    server::server(int max_msg_len, int max_conn, int max_backlog, int timeout_ms,
                   int thread_num, int listening_port, std::string listening_ip)

        : active_conn(0), max_msg_len(max_msg_len), max_conn(max_conn), max_backlog(max_backlog), timeout_ms(timeout_ms),
          thread_num(thread_num), listening_port(listening_port), listening_ip(listening_ip)
    {
        sockpp::socket::startup();
        io_workers = std::make_unique<threadpool::pool>(thread_num, timeout_ms);
        io_queue = std::make_unique<sockpp::io_queue<conn_ctx>>(thread_num,
            [this](std::shared_ptr<conn_ctx> conn,
                   std::unique_ptr<sockpp::io_ctx> io)
            {
                this->handle_io(conn, std::move(io));
            },
            [this](std::shared_ptr<conn_ctx> conn)
            {
                this->handle_close(conn);
            }
        );
    }

    server::~server()
    {
        sockpp::socket::cleanup();
    }

    /* Starts the infinite loop, listening for connections, and adding valid connections
    to the IO queue, so that future requests can be serviced by the work threadpool. */
    void server::start(void)
    {
        /* set all threads to listen for queued IO. */
        for (int i = 0; i < thread_num; i++) {
            io_workers->submit([this]()
            {
                try {
                    this->io_queue->listen();
                }
                catch (std::exception &ex) {
                    std::cout << ex.what() << std::endl;
                }
            });
        }

        /* start listening for incoming connections. */
        acc.open(listening_ip, listening_port, max_backlog);

        while (true) {
            sockpp::socket_t handle = acc.accept();
            sockpp::socket skt {handle};

            /* add connected socket handle to io queue. */
            std::shared_ptr<conn_ctx> conn = std::make_shared<conn_ctx>(handle, conn_default);
            io_queue->register_socket(handle, conn.get());
            connections.push_back(conn);

            /* make initial rx request. io context is freed when the request has been serviced.
            winsock tracks the address of the io_ctx for us, until io completion packet is queued.
            Freed by unique_ptr, or manually deleted if error encountered before queuing. */
            sockpp::io_ctx *const io = new sockpp::io_ctx(sockpp::io::rx);
            skt.rx(io, 1);
        }
    }

    void server::handle_io(std::shared_ptr<conn_ctx> conn, std::unique_ptr<sockpp::io_ctx> io)
    {
        if (io->type == sockpp::io::rx) {
            std::shared_ptr<req> req = parse_headers(io->buf);
            req->print();

            char ok[9] = "200 OK\r\n";
            sockpp::socket skt {conn->handle};
            sockpp::io_ctx *const tx_io = new sockpp::io_ctx(sockpp::io::rx);
            skt.tx(tx_io, 1);
        }
    }

    void server::handle_close(std::shared_ptr<conn_ctx> conn)
    {
        std::cout << "handle_close" << std::endl;
        return;
    }

    void server::validate(std::shared_ptr<req> req)
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

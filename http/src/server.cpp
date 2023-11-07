#include <http/server.h>
#include <http/http_parser.h>

#include <sockpp/socket.h>
#include <sockpp/io_ctx.h>

namespace http
{

    server::server(int max_msg_len, int max_conn, int max_backlog, int timeout_ms,
                   int thread_num, int listening_port, std::string &listening_ip)
        : active_conn(0), max_msg_len(max_msg_len), max_conn(max_conn), max_backlog(max_backlog), timeout_ms(timeout_ms),
          thread_num(thread_num), listening_port(listening_port), listening_ip(listening_ip)
    {
        sockpp::socket::startup();
        io_workers = std::make_unique<threadpool::pool>(thread_num, timeout_ms);

        auto on_io = [this](std::shared_ptr<conn_ctx> conn,
                            std::shared_ptr<sockpp::io_ctx> ctx)
        {
            this->handle_io(conn, ctx);
        };

        auto on_close = [this](void)
        {
            this->handle_close();
        };

        io_queue = std::make_unique<sockpp::io_queue<conn_ctx>>(nproc, on_io, on_close);
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
                this->io_queue->listen();
            });
        }

        /* start listening for incoming connections. */
        acc.open(listening_ip, listening_port, max_backlog);
        while (true) {
            sockpp::socket_t handle = acc.accept();
            sockpp::socket skt {handle};

            /* add connected socket handle to io queue. */
            std::shared_ptr<conn_ctx> conn = std::make_shared<conn_ctx>(handle, conn_default);
            connections.push_back(conn);
            io_queue->register_socket(handle, conn.get());

            /* make initial rx request. */
            std::shared_ptr<sockpp::io_ctx> ctx = io_queue->add_io_ctx(sockpp::io::rx);
            skt.rx(ctx, 1);
        }
    }

    void server::handle_io(std::shared_ptr<conn_ctx> conn, std::shared_ptr<sockpp::io_ctx> ctx)
    {
        if (ctx->type == sockpp::io::rx) {
            std::shared_ptr<struct req> req = parse_headers(ctx->buf);
            req->print();
            char ok[9] = "200 OK\r\n";
            sockpp::socket skt {conn->handle};
            std::shared_ptr<sockpp::io_ctx> tx_ctx = io_queue->add_io_ctx(sockpp::io::tx);
            skt.tx(tx_ctx, 1);
        }
    }

    void server::handle_close(void)
    {
        return;
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

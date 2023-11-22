#include <http/server.h>
#include <http/http_parser.h>
#include <http/http_err.h>

#include <sockpp/socket.h>
#include <sockpp/io_ctx.h>

#include <iostream>

namespace http
{

    server::server(int max_msg_len, int max_conn, int max_backlog, int timeout_ms,
                   int thread_num, int listening_port, const std::string &listening_ip)

        : active_conn(0), max_msg_len(max_msg_len), max_conn(max_conn), max_backlog(max_backlog), timeout_ms(timeout_ms),
          thread_num(thread_num), listening_port(listening_port), listening_ip(listening_ip),
          default_port_(default_port), default_scheme_(default_scheme)
    {
        sockpp::socket::startup();

        io_workers = std::make_unique<threadpool::pool>(thread_num, timeout_ms);
        io_queue = std::make_unique<sockpp::io_queue<conn_ctx>>(thread_num,
            [this](conn_ctx *conn, std::unique_ptr<sockpp::io_ctx> io)
            { this->handle_rx(conn, std::move(io)); },
            [this](conn_ctx *conn, std::unique_ptr<sockpp::io_ctx> io)
            { this->handle_tx(conn, std::move(io)); },
            [this](conn_ctx *conn) { this->handle_close(conn); }
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
                    this->io_queue->dequeue();
                }
                catch (std::exception &ex) {
                    std::cout << ex.what() << std::endl;
                }
            });
        }

        /* start listening for incoming connections. */
        acc.open(listening_ip, listening_port, max_backlog);

        /* other threads may delete connections, but can never create new ones.
        as such reading the size() is fine to do without synchronisation. */
        while (connections.size() < config_.max_concurrent_connections) {
            sockpp::socket_t handle = acc.accept();
            std::unique_ptr<sockpp::socket> skt = std::make_unique<sockpp::socket>(handle);

            /* add connected socket handle to io queue. */
            std::shared_ptr<conn_ctx> conn = std::make_shared<conn_ctx>(std::move(skt));
            add_conn(conn);

            io_queue->register_socket(handle, conn.get());

            /* make initial rx request. io context is freed when the request has been serviced.
            winsock tracks the address of the io_ctx for us, until io completion packet is queued.
            Freed by unique_ptr, or manually deleted if error encountered before queuing. */
            conn->rx();
        }
    }

    /* Responsible for handling transport layer rx completion. */
    void server::handle_rx(conn_ctx *conn_ptr, std::unique_ptr<sockpp::io_ctx> io)
    {
        std::shared_ptr<conn_ctx> conn = connections[conn_ptr->skt->handle()];

        conn->reassembly_buf += std::string(io->buf, io->bytes_rx);
        std::cout << conn->reassembly_buf << std::endl;

        if (conn->request->parse_state != content && conn->request->parse_state != chunk_trailers) {
            conn->parsed_to_idx = parse_headers(conn->reassembly_buf, conn->parsed_to_idx, conn->request.get());
        }

        if (conn->request->parse_state != content && conn->request->parse_state != chunk_trailers
            && !conn->request->has_err()) {
            /* We have not fully parsed the headers ==> issue an rx req for them. */
            conn->rx();
            return;
        }

        /* raw_content may not be the full content, we may need to make a new recv request to read all the content,
        if what we have seen so far is error free. */
        if (!conn->request->has_err()) {

            /* we have parsed the headers, let's check that what we have so far is sane. */
            

            parse_content(conn->reassembly_buf.substr(conn->parsed_to_idx), conn->request.get());
            if (conn->request->parse_state != complete && !conn->request->has_err()) {
                /* issue a new recv request to get the rest of the message content. */
                conn->rx();
                return;
            }
        }

        if (conn->request->has_err()) {
            conn->request->err->handle(conn);
            /* add the linger option, so that we don't get seg faults. */
            remove_conn(conn->skt->handle());
            return;
        }


        conn->reset_for_next();
        // this is where we need to hook in controller to process struct req, and
        // build the response.
        conn->tx("HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHowdy cowboy!");
    }
    
    void server::handle_tx(conn_ctx *conn_ptr, std::unique_ptr<sockpp::io_ctx> io)
    {
        std::shared_ptr conn = connections[conn_ptr->skt->handle()];
        if (io->bytes_tx < io->bytes_to_tx) {
            /* not all bytes have been tx'ed, issue a tx req to send the rest. */
            /* move to some sort of copy constructor. */
            sockpp::io_ctx *const tx_io = new sockpp::io_ctx(sockpp::io::tx);
            tx_io->write_buf(tx_io->buf + io->bytes_tx);
            tx_io->bytes_to_tx = io->bytes_to_tx;
            tx_io->bytes_tx = io->bytes_tx;
            conn->skt->tx(tx_io, 1);
        }
        
        if (!(conn->status & conn_keep_alive)) {
            std::cout << "closing the connection after successful response" << std::endl;
            handle_close(conn_ptr);
        }
        else {
            std::cout << "sending successful, listening for more rx's" << std::endl;
            conn->rx();
        }
    }

    void server::add_conn(std::shared_ptr<conn_ctx> conn)
    {
        std::unique_lock<std::mutex> lock(conn_mutex);
        connections[conn->skt->handle()] = conn;
    }

    void server::remove_conn(sockpp::socket_t conn_handle)
    {
        std::unique_lock<std::mutex> lock(conn_mutex);
        connections.erase(conn_handle);
    }

    /* Client has initiated a graceful close of the connection. */
    void server::handle_close(conn_ctx *conn)
    {
        conn->status |= conn_rx_closed;
        conn->close_tx();
        remove_conn(conn->skt->handle());
    }

}

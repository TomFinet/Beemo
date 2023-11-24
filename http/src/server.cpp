#include <http/server.h>
#include <http/http_parser.h>
#include <http/http_err.h>
#include <http/routing.h>

#include <sockpp/socket.h>
#include <sockpp/io_ctx.h>

#include <iostream>

namespace http
{

    server::server(const struct config &config) : config_(config)
    {
        logger_ = spdlog::stdout_color_mt(config_.logger_name);

        sockpp::socket::startup();

        io_workers = std::make_unique<threadpool::pool>(config_.num_req_handler_threads, config_.req_timeout_ms);
        io_queue = std::make_unique<sockpp::io_queue<conn_ctx>>(config_.num_req_handler_threads,
            [this](conn_ctx *conn, std::unique_ptr<sockpp::io_ctx> io)
            { this->handle_rx(conn, std::move(io)); },
            [this](conn_ctx *conn, std::unique_ptr<sockpp::io_ctx> io)
            { this->handle_tx(conn, std::move(io)); },
            [this](conn_ctx *conn) { this->remove_conn(conn->skt->handle()); }
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
        logger_->info("Starting HTTP server.");

        /* set all threads to listen for queued IO. */
        for (int i = 0; i < config_.num_req_handler_threads; i++) {
            io_workers->submit([this]()
            {
                try {
                    this->io_queue->dequeue();
                }
                /* if we actually hit a serious exception, gracefully kill connection.
                TODO: create http_exception class with connection as a member. */
                catch (std::exception &ex) {
                    std::cout << ex.what() << std::endl;
                }
            });
        }

        /* start listening for incoming connections. */
        acc.open(config_.listening_ip, config_.listening_port, config_.max_req_backlog);

        /* other threads may delete connections, but can never create new ones.
        as such reading the size() is fine to do without synchronisation. */
        /* TODO: we don't want to exit the loop when this happens, we want to
        sleep and be woken up when the condition is again met: screams condition variable. */
        while (connections.size() < config_.max_concurrent_connections) {
            sockpp::socket_t handle = acc.accept();
            std::unique_ptr<sockpp::socket> skt = std::make_unique<sockpp::socket>(handle);

            /* add connected socket handle to io queue. */
            std::shared_ptr<conn_ctx> conn = std::make_shared<conn_ctx>(std::move(skt));
            add_conn(conn);

            io_queue->register_socket(handle, conn.get());
            logger_->info("New connection established.");

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
        std::unique_ptr<struct response> response; 

        conn->reassembly_buf += std::string(io->buf, io->bytes_rx);

        if (conn->request->parse_state != content && conn->request->parse_state != chunk_trailers) {
            conn->parsed_to_idx = parse_headers(conn->reassembly_buf, conn->parsed_to_idx, conn->request.get());
        }

        if (conn->request->parse_state != content && !conn->request->has_err()) {
            /* We have not fully parsed the headers ==> issue an rx req for them. */
            goto rx;
        }

        /* raw_content may not be the full content, we may need to make a new recv request to read all the content,
        if what we have seen so far is error free. */
        if (!conn->request->has_err()) {

            validate(conn->request.get(), config_);
            if (conn->request->has_err()) {
                goto err;
            }
            
            parse_content(conn->reassembly_buf.substr(conn->parsed_to_idx), conn->request.get());

            if (conn->request->parse_state != complete && !conn->request->has_err()) {
                /* issue a new recv request to get the rest of the message content. */
                goto rx;
            }
        }

        if (conn->request->has_err()) {
            goto err;
        }

        /* there are no errors, offloading the request to the handler for the request uri. */ 
        response = route_to_resource_handler(conn->request.get());
        if (conn->request->has_err()) {
            goto err;
        }

        conn->reset_for_next();
        conn->tx(std::move(response));
        return;

    err:
        /* TODO: add the linger option, so that we don't get seg faults. */
        conn->request->err->handle(conn, logger_);
        conn->close_tx();
        return;
    rx:
        conn->rx();
        return;
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
            remove_conn(conn->skt->handle());
        }
        else {
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

}

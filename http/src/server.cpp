#include <http/server.h>
#include <http/parser.h>
#include <http/err.h>
#include <http/routing.h>


namespace http
{

    server::server(const struct config &config) : config_(config)
    {
        logger_ = spdlog::stdout_color_mt(config_.logger_name);

        sockpp::socket::startup();

        io_workers = std::make_unique<threadpool::pool>(config_.num_req_handler_threads, config_.req_timeout_ms);
        io_queue = std::make_unique<sockpp::io_queue>(
            [this](sockpp::socket_t skt_handle) { this->handle_rx(skt_handle); },
            [this](sockpp::socket_t skt_handle) { this->handle_tx(skt_handle); },
            [this](sockpp::socket_t skt_handle) { this->remove_conn(skt_handle); }
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
                    logger_->error(ex.what());
                }
            });
        }

        /* start listening for incoming connections. */
        acc.open(config_.listening_ip, config_.listening_port, config_.max_connection_backlog,
                 config_.max_linger_sec, config_.rx_buf_len);

        /* other threads may delete connections, but can never create new ones.
        as such reading the size() is fine to do without synchronisation. */
        while (true) {
            sockpp::socket_t skt_handle = acc.accept();
            std::shared_ptr<connection> conn = std::make_shared<connection>(skt_handle);

            logger_->info("Connections: {0:d}", connections.size());

            if (connections.size() < config_.max_concurrent_connections) {
                add_conn(conn);
                io_queue->register_socket(skt_handle, &conn->transport_conn);
                logger_->info("New connection established.");
                conn->rx();
            }
        }
    }

    /* Responsible for handling transport layer rx completion. */
    /* TODO: simplify all the branches. */
    void server::handle_rx(sockpp::socket_t skt_handle)
    {
        std::shared_ptr<connection> conn = connections[skt_handle];

        if (conn->request->parse_state != content && conn->request->parse_state != chunk_trailers) {
            conn->parsed_to_idx = parse_headers(conn->transport_conn.rx_buf, conn->request.get(), {conn->parsed_to_idx, &config_});
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
            
            parse_content(conn->transport_conn.rx_buf.substr(conn->parsed_to_idx), conn->request.get());

            if (conn->request->parse_state != complete && !conn->request->has_err()) {
                /* issue a new recv request to get the rest of the message content. */
                goto rx;
            }
        }

        if (conn->request->has_err()) {
            goto err;
        }

        /* there are no errors, offloading the request to the handler for the request uri. */ 
        conn->res = route_to_resource_handler(conn->request.get());
        if (conn->request->has_err()) {
            goto err;
        }

        conn->reset_for_next();
        conn->tx();
        return;

    err:
        conn->request->err->handle(conn, logger_);
        conn->close_tx();
        return;
    rx:
        conn->rx();
        return;
    }
    
    void server::handle_tx(sockpp::socket_t skt_handle)
    {
        std::shared_ptr<connection> conn = connections[skt_handle];

        if (!conn->keep_alive()) {
            remove_conn(conn->transport_conn.skt->handle());
        }
        else {
            conn->rx();
        }
    }

    void server::add_conn(std::shared_ptr<connection> conn)
    {
        std::unique_lock<std::mutex> lock(conn_mutex);
        connections[conn->transport_conn.skt->handle()] = conn;
    }

    void server::remove_conn(sockpp::socket_t skt_handle)
    {
        std::unique_lock<std::mutex> lock(conn_mutex);
        connections.erase(skt_handle);
    }

}

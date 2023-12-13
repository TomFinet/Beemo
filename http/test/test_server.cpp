#include <http/server.h>
#include <http/msg.h>
#include <http/routing.h>
#include <http/parser.h>

struct http::config config;

std::string io_buf_overflow_handler(struct req *req) {
    return std::string(2000, 'a');
}

int main(void)
{
    http::register_req_handler({"/", http::get}, (http::req_handler_t)&io_buf_overflow_handler);
    http::server server(config);
    server.start();
}
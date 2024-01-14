#include <http/server.h>
#include <http/msg.h>
#include <http/routing.h>
#include <http/parser.h>

struct config config;

std::string io_buf_overflow_handler(struct req *req) {
    return std::string(2000, 'a');
}

int main(void)
{
    register_req_handler({"/", get}, (req_handler_t)&io_buf_overflow_handler);
    server server(config);
    server.start();
}
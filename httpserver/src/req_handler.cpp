#include <iostream>
#include <thread>
#include <stdexcept>

#include <httpserver/req_handler.h>

#include <httparser/parse_error.h>
#include <httparser/http_parser.h>
#include <httparser/http_req.h>


namespace httpserver {

void req_handler::handle(std::string &pkt)
{
    httparser::http_parser parser;
    httparser::http_req req;

    try {
        parser.parse(parser.lex(pkt), req);
    }
    catch (httparser::parse_error &parse_err) {
        std::cout << "tid: " << std::this_thread::get_id()
                  << " | " << parse_err.what() << std::endl;
    }
    catch (std::runtime_error &runtime_err) {
        std::cout << "tid: " << std::this_thread::get_id()
                  << " | " << runtime_err.what() << std::endl;
    }
}

}
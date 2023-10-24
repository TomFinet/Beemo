#include <thread>
#include <stdexcept>

#include <httpserver/req_handler.h>

#include <httparser/parse_error.h>
#include <httparser/http_parser.h>
#include <httparser/http_req.h>

#include <uriparser/uri_error.h>

#include <iostream>

namespace httpserver {

void req_handler::handle(std::string &pkt)
{
    http::http_parser parser;

    try {
        parser.parse(parser.lex(pkt));

        std::cout << pkt << std::endl << std::endl;

        std::cout << "method: " << parser.req.method << std::endl;
        std::cout << "version: " << parser.req.version.major << "." << parser.req.version.minor << std::endl;
        std::cout << "reg_name: " << parser.req.uri.reg_name << std::endl; 

    }
    catch (http::parse_error &parse_err) {
        print_error(parse_err);
    }
    catch (uri::uri_error &uri_err) {
        print_error(uri_err);
    }
    catch (std::invalid_argument &arg_err) {
        print_error(arg_err);
    }
    catch (std::out_of_range &range_err) {
        print_error(range_err);
    }
    catch (std::runtime_error &runtime_err) {
        print_error(runtime_err);
    }
    catch (std::exception &ex) {
        print_error(ex);
    }
}

}
#include <iostream>
#include <vector>
#include <stdexcept>
#include <thread>

#include <httparser/http_parser.h>
#include <httparser/http_error.h>

#include <uriparser/uri_error.h>

int main() {

    std::vector<http::line_t> get = {
        {"GET", "/dsf/pa1h", "HTTP/1.1"},
        {"Connection:", "keep-alive"},
        {"Transfer-Encoding:", "chunked"},
        {"Host:", "www.google.com"},
        {"Test", "body..."}
    };

    http::http_parser parser;
    http::http_req req;

    try {
        parser.parse(get);
    }
    catch (http::http_error &http_err) {
        std::cout << "tid: " << std::this_thread::get_id()
                  << " | " << http_err.what() << std::endl;
    }
    catch (uri::uri_error &uri_err) {
        std::cout << "tid: " << std::this_thread::get_id()
                  << " | " << uri_err.what() << std::endl;
    }
    catch (std::invalid_argument &arg_err) {
        std::cout << "tid: " << std::this_thread::get_id()
                  << " | " << arg_err.what() << std::endl;
    }
    catch (std::out_of_range &range_err) {
        std::cout << "tid: " << std::this_thread::get_id()
                  << " | " << range_err.what() << std::endl;
    }
    catch (std::runtime_error &runtime_err) {
        std::cout << "tid: " << std::this_thread::get_id()
                  << " | " << runtime_err.what() << std::endl;
    }
    catch (std::exception &ex) {
        std::cout << "tid: " << std::this_thread::get_id()
                  << " | " << ex.what() << std::endl;
    }
   
    return 0;
}
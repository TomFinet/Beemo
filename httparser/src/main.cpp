#include <iostream>
#include <vector>
#include <stdexcept>
#include <thread>

#include <httparser/http_parser.h>
#include <httparser/parse_error.h>

int main() {

    std::vector<httparser::line_t> get = {
        {"GET", "http://www.google.com", "HTTP/1.1"},
        {"Connection:", "keep-alive"},
        {"Transfer-Encoding:", "chunked"},
        {"Host:", "www.google.com"},
        {"Test", "body..."}
    };

    httparser::http_parser parser;
    httparser::http_req req;

    try {
        parser.parse(get, req);
    }
    catch (httparser::parse_error &parse_err) {
        std::cout << "tid: " << std::this_thread::get_id()
                  << " | " << parse_err.what() << std::endl;
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
#include <thread>
#include <stdexcept>

#include <httpserver/req_worker.h>

#include <http/error.h>
#include <http/msg.h>

#include <uriparser/uri_error.h>

#include <iostream>

namespace httpserver {

void req_worker::work(std::string &pkt)
{

    try {
        
        std::cout << pkt << std::endl << std::endl;

        http::req req(pkt);

        std::cout << "method: " << req.method << std::endl;
        std::cout << "version: " << req.version.major << "." << req.version.minor << std::endl;
        std::cout << "reg_name: " << req.uri.reg_name << std::endl; 
        std::cout << "port: " << req.uri.port << std::endl;
        std::cout << "scheme: " << req.uri.scheme << std::endl;
        std::cout << "path: " << req.uri.path << std::endl << std::endl;

        req.validate();

    }
    catch (http::error &http_err) {
        // respond http bad request
        print_error(http_err);
    }
    catch (uri::uri_error &uri_err) {
        // respond http bad request
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
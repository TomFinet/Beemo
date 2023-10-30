#include <thread>
#include <stdexcept>
#include <iostream>

#include <http/req_worker.h>
#include <http/error.h>
#include <http/msg.h>
#include <http/req_parser.h>

#include <uri/uri_error.h>


namespace http {

    void req_worker::work(std::string &pkt)
    {
        try {
            std::cout << pkt << std::endl << std::endl;

            struct req req; 
            req_parser parser(&req); 
            parser.parse(pkt);
            req.print();
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
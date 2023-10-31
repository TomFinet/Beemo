#include <thread>
#include <stdexcept>
#include <iostream>

#include <http/req_worker.h>
#include <http/msg.h>
#include <http/req_parser.h>


namespace http {

    void req_worker::work(std::string &pkt)
    {
        try {
            struct req req; 
            req_parser parser(&req); 
            parser.parse(pkt);
            req.print();
        }
        catch (std::domain_error &ex) {
            print_error(ex);
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
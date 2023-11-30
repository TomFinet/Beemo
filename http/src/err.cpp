#include <http/err.h>

#include <algorithm>
#include <string_view>
#include <sstream>

namespace http
{

    err_response_handler bad_req_handler {"Bad Request", 400};
    err_response_handler not_found_handler {"Not Found", 404};
    err_response_handler method_not_allowed_handler {"Method Not Allowed", 405};
    err_response_handler timeout_handler {"Request Timeout", 408};
    err_response_handler len_required_handler {"Length Required", 411};
    err_response_handler uri_too_long_handler {"URI Too Long", 414};
    err_response_handler unsupported_media_type_handler {"Unsupported Media Type", 415};
    err_response_handler misdirected_req_handler {"Misdirected Request", 421};

    err_response_handler not_impl_handler {"Not Implemented", 501};


    void err_response_handler::handle(std::shared_ptr<connection> conn, std::shared_ptr<spdlog::logger> logger)
    {
        logger->error("Error {0:d} {1}", status_code, reason);
        conn->reset_for_next();
        conn->res_->status_code = status_code;
        conn->res_->reason = reason;
        conn->res_->add_header("Content-Length", "0");
        conn->res_->add_header("Connection", "close");
        conn->tx(); 
        conn->close_tx();
    }

}
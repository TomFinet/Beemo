#include <http/http_err.h>

#include <sockpp/io_ctx.h>

#include <algorithm>
#include <string_view>
#include <sstream>

namespace http
{

    err_response_handler bad_req_handler {"Bad Request", 400};
    err_response_handler method_not_allowed_handler {"Method Not Allowed", 405};
    err_response_handler timeout_handler {"Request Timeout", 408};
    err_response_handler len_required_handler {"Length Required", 411};
    err_response_handler uri_too_long_handler {"URI Too Long", 414};
    err_response_handler unsupported_media_type_handler {"Unsupported Media Type", 415};
    err_response_handler misdirected_req_handler {"Misdirected Request", 421};

    err_response_handler not_impl_handler {"Not Implemented", 501};


    void err_response_handler::handle(std::shared_ptr<conn_ctx> conn, std::shared_ptr<spdlog::logger> logger)
    {
        logger->error("Error {0:d} {1}", status_code, reason);
        conn->reset_for_next();
        std::stringstream ss;
        ss << "HTTP/1.1 " << status_code << " " << reason << "\r\nContent-Length: 0\r\n\r\n";
        conn->close_rx();
        conn->tx(ss.str()); 
    }

}
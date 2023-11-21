#include <http/http_err.h>

#include <sockpp/io_ctx.h>

#include <algorithm>
#include <string_view>

namespace http
{

    err_conn_close_handler close_handler;
    err_response_handler bad_req_handler {"Bad Request", 400};
    err_response_handler len_required_handler {"Length Required", 411};
    err_response_handler not_impl_handler {"Not Implemented", 501};

    err_response_handler::err_response_handler(const std::string &reason, unsigned short status_code)
        : status_code(status_code)
    {
        strncpy_s(this->reason, reason.c_str(), std::min(reason.size(), max_reason_len)); 
    }
    
    void err_conn_close_handler::handle(std::shared_ptr<conn_ctx> conn)
    {
        conn->close();
    }

    void err_response_handler::handle(std::shared_ptr<conn_ctx> conn)
    {
        conn->reset_for_next();
        conn->tx("HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n"); 
    }
}
#include <http/err.h>

namespace beemo
{
    err_builder bad_req_handler {"Bad Request", 400};
    err_builder not_found_handler {"Not Found", 404};
    err_builder method_not_allowed_handler {"Method Not Allowed", 405};
    err_builder timeout_handler {"Request Timeout", 408};
    err_builder len_required_handler {"Length Required", 411};
    err_builder uri_too_long_handler {"URI Too Long", 414};
    err_builder unsupported_media_type_handler {"Unsupported Media Type", 415};
    err_builder misdirected_req_handler {"Misdirected Request", 421};

    err_builder internal_server_handler {"Internal Server Error", 500};
    err_builder not_impl_handler {"Not Implemented", 501};

    void err_builder::build(resp*const resp) const
    {
        resp->status_code = status_code_;
        resp->reason = reason_;
        resp->put_header("Content-Length", "0");
        resp->put_header("Connection", "close"); 
    }
}
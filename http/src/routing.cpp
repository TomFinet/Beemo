#include <http/routing.h>
#include <http/config.h>
#include <http/http_err.h>

#include <unordered_map>


namespace http
{

    /* Users of this API will register request handlers with this map. */
    typedef std::unordered_map<struct req_id, req_handler_t, struct req_hasher> req_handler_map_t; 

    req_handler_map_t req_to_handler;


    void validate(req *const req, const struct config &config)
    {
        if (req->version.major != supported_http_major_version ||
            req->version.minor != supported_http_minor_version) {
            goto err_bad_req;
        }

        if (req->fields[host_header].empty()) {
            goto err_bad_req;
        }

        if (!req->uri.scheme.empty() && req->uri.scheme != config.default_uri_scheme) {
            goto err_bad_req;
        }

        if (!req->uri.userinfo.empty()) {
            goto err_bad_req;
        }

        if (req->uri.port != config.listening_port || 
           (req->uri.ipv4 != config.listening_ip &&
            req->uri.reg_name != config.listening_reg_name)) {
            goto err_misdirected_req;
        }

        return;

    err_misdirected_req:
        req->err = &misdirected_req_handler;
        return;
    err_bad_req:
        req->err = &bad_req_handler;
        return;
    }

    void route_to_resource_handler(req *const req)
    {
        /* we are going to parse the req uri path, lookup a registered handler
        for that path and request method. If it exists let it execute, else respond with error code. */
        req_id req_id {req->uri.path, req->method};

        if (!req_to_handler.contains(req_id)) {
            goto err_not_found;
        }

        /* call the request handler. */
        req_to_handler[req_id](req);

        return;

    err_not_found:
        req->err = &bad_req_handler;
        return;
    }

    void register_req_handler(const req_id &req_id, req_handler_t req_handler)
    {
        req_to_handler[req_id] = req_handler;
    }
}
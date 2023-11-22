#include <http/routing.h>
#include <http/config.h>
#include <http/http_err.h>


namespace http
{

    void validate(req *const req, const struct config &config)
    {
        if (req->version.major != supported_http_major_version ||
            req->version.minor != supported_http_minor_version) {

        }

        if (req->fields[host_header].empty()) {

        }

        if (!req->uri.scheme.empty() && req->uri.scheme != config.default_uri_scheme) {

        }

        if (!req->uri.userinfo.empty()) {
            
        }

        if (req->uri.port != config.listening_port) {
            goto err_misdirected_req;
        }

        if (req->uri.ipv4 != config.listening_ip && req->uri.reg_name != config.listening_reg_name) {
            goto err_misdirected_req;
        }

    err_misdirected_req:
        req->err = &misdirected_req_handler;
        return;

    }

}
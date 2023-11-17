#include <http/http_semantics.h>
#include <http/server.h>

#include <stdexcept>


namespace http
{
    /* Parses the message fields, and checks that they are valid. */
    void validate(req *const req)
    {
        if (req->version.major != 1 || req->version.minor != 1) {
            throw std::domain_error("Server only supports HTTP 1.1");
        }

        if (req->fields[host_header].empty()) {
            throw std::domain_error("Host header field cannot be missing.");
        }

        if (!req->uri.scheme.empty() && req->uri.scheme != default_scheme) {
            throw std::domain_error("Unsupported scheme.");
        }

        if (!req->uri.userinfo.empty()) {
            throw std::domain_error("HTTP has depricated userinfo.");
        }
    }

    void chunked_content()
    {
        
    }

}
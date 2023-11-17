#include <string_view>

#include <http/msg.h>

namespace http {
    
    /* returns string view encompassing req content rx'ed at time of call. */
    size_t parse_headers(std::string_view raw_req, size_t start_idx, req *const req);
    void parse_req_line(std::string_view req_line, req *const req);
    void parse_req_target(std::string_view req_target, req *const req);
    void parse_version(std::string_view version, req *const req);
    void parse_field_line(std::string_view field_line, req *const req);
    void parse_field_host(std::string_view host_val, req *const req);

    void parse_content(size_t content_start_idx, req *const req);

}
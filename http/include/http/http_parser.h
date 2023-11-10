#include <memory>
#include <string_view>

#include <http/msg.h>

namespace http {

    std::unique_ptr<req> parse_headers(std::string_view raw_req);
    void parse_req_line(std::string_view req_line, req *const req);
    void parse_req_target(std::string_view req_target, req *const req);
    void parse_version(std::string_view version, req *const req);
    void parse_field_line(std::string_view field_line, req *const req);
    void parse_field_host(std::string_view host_val, req *const req);
    void parse_body(std::string_view body, req *const req);
    void validate(req *const req);

}
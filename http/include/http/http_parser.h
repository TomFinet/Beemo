#include <memory>
#include <string_view>

#include <http/msg.h>

namespace http {

    std::shared_ptr<struct req> parse_headers(std::string_view raw_req);
    void parse_req_line(std::string_view req_line, std::shared_ptr<struct req> req);
    void parse_req_target(std::string_view req_target, std::shared_ptr<struct req> req);
    void parse_version(std::string_view version, std::shared_ptr<struct req> req);
    void parse_field_line(std::string_view field_line, std::shared_ptr<struct req> req);
    void parse_field_host(std::string_view host_val, std::shared_ptr<struct req> req);
    void parse_body(std::string_view body, std::shared_ptr<struct req> req);
    void validate(std::shared_ptr<struct req> req);

}
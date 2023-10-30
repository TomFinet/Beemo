#pragma

#include <string_view>

#include <http/msg.h>


namespace http {

    typedef std::string token_t;
    typedef std::vector<token_t> line_t;

    /* Responsible for parsing a single http request.
       One request parser is constructed per request. */
    class req_parser {

        public:

            req_parser(struct req *const req) : req(req) { }
            ~req_parser() = default;

            req_parser(const req_parser &parser) = delete;
            req_parser(req_parser &&parser) = delete;

            req_parser& operator=(const req_parser &parser) = delete; 
            req_parser& operator=(req_parser &&parser) = delete;

            /* Pointer to request object we are parsing into.
               The request is on stack. */
            struct req *req;

            /**
            * Parses a tokenised request into an @http_req object.
            * according to RFC 9112 sect 2.1.
            *
            * @param req the parsed HTTP request object.
            */
            void parse(std::string_view raw_req);

            void parse_req_line(std::string_view req_line);

            void parse_req_target(std::string_view req_target);

            void parse_version(std::string_view version);
            
            void parse_field_line(std::string_view field_line);

            void parse_header_host(std::string_view host_val);

            void parse_body(std::string_view body);

            void validate();
    };

}
#include <algorithm>

#include <http/keyword_map.h>
#include <http/req_parser.h>
#include <http/error.h>


namespace {

    constexpr auto carriage_ret = '\r';
    constexpr auto line_feed = '\n';
    constexpr auto space = ' ';
    constexpr auto crlf = carriage_ret + line_feed;

    constexpr int version_len = 8;
    constexpr int version_major_start = 5;
    constexpr int version_minor_start = 7;

    constexpr int default_port = 80;
    constexpr auto &default_scheme = "http";

    unsigned int keyword_val(const http::token_t &keyword, const std::string &err_msg)
    {
        struct http_kvp *kvp = http_keyword_map::in_word_set(keyword.c_str(),
            static_cast<unsigned int>(keyword.length()));

        if (kvp == NULL) {
            throw http::error(err_msg);
        }

        return kvp->value;
    }
}

namespace http {
    
    void req_parser::parse(std::string_view raw_req)
    {
        if (raw_msg.empty()) {
            throw error("Request is empty.")
        }

        if (req->status == unparsed) {

            const size_t req_line_end = raw_req.find(crlf);
            if (req_line_end == std::string_view:npos) {
                throw error("Request line must end with a carriage return line feed.");
            }

            std::string_view req_line(raw_req, req_line_end); 
            parse_req_line(req_line); 

            size_t line_start = req_line_end + 2;
            while (true) {

                size_t line_end = raw_req.find(crlf, line_start);

                if (line_end == std::string_view::npos) {
                    throw error("Bodyless request must end with a crlf.");
                }

                if (line_end == line_start) {
                    req->status = headers;
                    return;
                }

                std::string_view field_line(raw_req.begin() + line_start, raw_req.begin() + line_end);
                parse_field_line(field_line);

                line_start = line_end + 2;
            }
        }
        else if (req->status == headers) {
            // parsing the body here 
            
        }
   }

    void req_parser::parse_req_line(std::string_view req_line)
    {
        if (req_line.empty()) {
            throw error("Request line cannot be empty.");
        }

        const size_t method_end = req_line.find(space);
        if (method_end == std::string_view::npos) {
            throw error("Request line method must be delimited by a space.");
        }
        std::string method = req_line.substr(method_end);
        req->method = method;

        const size_t req_target_end = req_line.find(space, method_end + 1);
        if (req_target_end == std::string_view::npos) {
            throw error("Request line request target must be delimited by a space.");
        }
        std::string_view req_target(req_line.begin() + method_end + 1, req_line.begin() + req_target_end);
        parse_req_target(req_target);

        if (req_target_end + 1 >= req_line.size()) {
            throw error("Request line version cannot be empty.");
        }
        std::string_view version(req_line.begin() + req_target_end + 1, req_line.end());
        parse_version(version);
    }

    void req_parser::parse_req_target(std::string_view req_target)
    {
        if (req_target.at(0) == '*') {
            req->target_form = http::asterik;
            if (req_target.length() > 1 || req_target.at(0) != '*') {
                req->uri.asterik = false;
                throw error("Invalid request target.");
            }
            req->uri.asterik = true; 
        }
        else if (req_target.at(0) == '/') {
            req->target_form = http::origin;
            req->uri.parse_uri(req_target, uri::flag_path | uri::flag_query);
        }
        else if (req_target.find('/') != std::string::npos) {
            req->target_form = http::absolute;
            req->uri.parse_uri(req_target, uri::flag_all);
        }
        else {
            req->target_form = http::authority;
            req->uri.parse_uri(req_target, uri::flag_authority);
        }
    }

    void req_parser::parse_version(std::string_view version)
    {
        if (version.size() != version_len) {
            throw error("HTTP version must be 8 characters long.");
        }

        req->version.major = std::stoi(static_cast<std::string>(version.substr(version_major_start, 1)));
        req->version.minor = std::stoi(static_cast<std::string>(version.substr(version_minor_start, 1)));
    }

    /* we ignore any unrecognised field lines. */
    void req_parser::parse_field_line(std::string_view field_line)
    {
        /* field[0] ends with ':', so we remove it. */
        std::string_view field_key(field_line.begin(), field_line.end() - 1);

        if (field_key == connection_header) { 
            req->fields[connection_header] = field_line;
        }
        else if (field_key == encoding_header) { 
            req->fields[encoding_header] = field_line;
        }
        else if (field_key == host_header) {
            parse_header_host(field_line);
        }
    }

    void req_parser::parse_header_host(std::string_view host_val)
    {
        switch (req->target_form) {
            case http::origin:
                /* req_target contains path and query. */
                req->uri.parse_uri(host_val, uri::flag_scheme | uri::flag_authority);
                break;
            case http::authority:
                /* req_target contains host and port. */
                req->uri.parse_uri(host_val, ~uri::flag_authority);
                break;
            case http::asterik:
                /* req_target contains */
                req->uri.parse_uri(host_val, uri::flag_scheme | uri::flag_authority);
                break;
        }

        if (req->uri.scheme == uri::no_scheme) {
            req->uri.scheme = default_scheme;
        }

        if (req->uri.port = uri::no_port) {
            req->uri.port = default_port;
        }
    }

    void req_parser::parse_body(std::string_view body)
    {
        req->body = body;
    }

    void req_parser::validate()
    {
        if (req->version.major != 1 || req->version.minor != 1) {
            throw error("Server only supports HTTP 1.1");
        }

        if (req->fields[host_header].empty()) {
            throw error("Host header field cannot be missing.");
        }

        if (req->uri.scheme != default_scheme) {
            throw error("Unsupported scheme.");
        }

        if (req->uri.userinfo.empty()) {
            throw error("HTTP has depricated userinfo.");
        }
    }
}
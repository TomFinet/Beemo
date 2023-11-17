#include <string>
#include <stdexcept>
#include <ranges>
#include <algorithm>

#include <uri/uri_parser.h>

#include <http/http_err.h>
#include <http/http_parser.h>
#include <http/keyword_map.h>
#include <http/msg.h>

#include <utils/parsing.h>


namespace
{

    constexpr auto space = ' ';
    constexpr auto crlf = "\r\n"; 

    constexpr int version_len = 8;
    constexpr int version_major_start = 5;
    constexpr int version_minor_start = 7;

    constexpr auto &content_length = "content-length";
    constexpr auto &transfer_encoding = "transfer-encoding";

    int keyword_val(std::string_view keyword)
    {
        std::string keyword_str(keyword);
        struct http_kvp *kvp = http_keyword_map::in_word_set(keyword_str.c_str(),
            static_cast<unsigned int>(keyword.size()));

        if (kvp == nullptr) {
            return 0;
        } 
        return kvp->value;
    }

    std::string lowercase_str(std::string_view s)
    {
        auto lowered = std::views::transform(s, std::tolower); 
        return std::string(lowered.begin(), lowered.end());
    }
    
    /* once we encounter an error, we want to read up to the end of the request.
    so that we flush the bad request out of our rx buffers. */
    void parse_to_req_end(void)
    {

    }

    void set_req_err(http::req *const req, const err_handler *const err_handler)
    {
        if (!req->has_err()) {
            req->err = err_handler;
            parse_to_req_end();
        }
    }

    void parse_transfer_encoding(struct http::req *const req)
    {
        if (!req->fields.contains(transfer_encoding)) {
            return;
        }

        std::string encodings = req->fields[transfer_encoding];

        for (std::string::const_iterator it = encodings.begin(); it != encodings.end();) {

            std::string::const_iterator start = it;
            while (it != encodings.end() && *it != ',') {
                it++;
            }
            std::string::const_iterator end = it;

            if (start == end) {
                goto err_response;
            }

            std::string encoding_str(start, end);
            http::encoding_t encoding = static_cast<http::encoding_t>(keyword_val(encoding_str));
            if (encoding == http::encoding_invalid) {
                goto err_response;
            }
            req->transfer_encodings.push(encoding);
            if (encoding == http::chunked && it != encodings.end()) {
                goto err_response;
            }

            if (end != encodings.end()) {
                it = end + 1;
                if (end + 1 != encodings.end() && *(end + 1) == space) {
                    it++;
                }
            }
        }
        return;

    err_response:
        req->err = &http::bad_req_handler;
    }

    void parse_content_len(struct http::req *const req)
    {
        if (!req->fields.contains(content_length) || req->fields.contains(transfer_encoding)) {
            return;
        }

        std::string_view content_len_view {req->fields[content_length]}; 
        utils::parse_pattern(content_len_view.begin(), content_len_view.end(), std::isdigit);
        std::string content_len {content_len_view};
        req->content_len = std::stoi(content_len);
    }

    void parse_connection(struct http::req *const req)
    {
        // parse the connection field. connection is kept alive by default, unless the connection: close is provided here.
    }
}


namespace http
{

    size_t parse_headers(std::string_view raw_req, size_t start_idx, req *const req)
    {
        std::string_view req_line;
        std::string_view field_line;
        size_t line_start = start_idx;
        size_t line_end;
        size_t req_line_end;
        size_t parsed_up_to = start_idx;

        if (req->parse_state == start_line) {
            req_line_end = raw_req.find(crlf);
            if (req_line_end == std::string_view::npos) {
                set_req_err(req, &bad_req_handler);
            }

            req_line = {raw_req.begin(), raw_req.begin() + req_line_end}; 
            line_start = req_line_end + 2;
            parse_req_line(req_line, req); 
        }

        while (req->parse_state == headers) {
            line_end = raw_req.find(crlf, line_start);

            if (line_end == std::string_view::npos) {
                /* rest of headers may not have been rx'ed yet. */
                parsed_up_to = line_start;
                goto ret;
            }

            if (line_end == line_start) {
                req->parse_state = content;
                parsed_up_to = line_start;
                if (line_start + 2 < raw_req.size()) {
                    parsed_up_to += 2;
                }
            }
            else {
                field_line = {raw_req.begin() + line_start, raw_req.begin() + line_end};
                parse_field_line(field_line, req);

                line_start = line_end + 2;
            }
        }
        goto ret;

    err_response:
        req->err = &bad_req_handler;
        goto ret;
    err_close:
        req->err = &close_handler;
    ret:
        return parsed_up_to;
   }

    void parse_req_line(std::string_view req_line, req *const req)
    {
        size_t method_end;
        size_t req_target_end;
        std::string_view req_target;
        std::string_view version;

        if (req_line.empty()) {
            goto err_response;
        }

        method_end = req_line.find(space);
        if (method_end == std::string_view::npos) {
            goto err_response;
        }
        req->method = static_cast<method_t>(keyword_val(req_line.substr(0, method_end)));
        if (req->method == method_invalid) {
            goto err_response;
        }

        req_target_end = req_line.find(space, method_end + 1);
        if (req_target_end == std::string_view::npos) {
            goto err_response;
        }
        req_target = {req_line.begin() + method_end + 1, req_line.begin() + req_target_end};
        parse_req_target(req_target, req);

        if (req_target_end + 1 >= req_line.size()) {
            goto err_response;
        }
        version = {req_line.begin() + req_target_end + 1, req_line.end()};
        parse_version(version, req);

        req->parse_state = headers;
        return;

    err_response:
        req->err = &bad_req_handler;
    }

    void parse_req_target(std::string_view req_target, req *const req)
    {
        if (req_target.at(0) == '*') {
            req->target_form = http::asterik;
            if (req_target.length() > 1 || req_target.at(0) != '*') {
                req->uri.asterik = false; 
                req->err = &bad_req_handler;
                return;
            }
            req->uri.asterik = true; 
        }
        else if (req_target.at(0) == '/') {
            req->target_form = http::origin;
            uri::parse_uri(&req->uri, req_target, uri::flag_path | uri::flag_query);
        }
        else if (req_target.find('/') != std::string::npos) {
            req->target_form = http::absolute;
            uri::parse_uri(&req->uri, req_target, uri::flag_all);
        }
        else {
            req->target_form = http::authority;
            uri::parse_uri(&req->uri, req_target, uri::flag_authority);
        }
    }

    void parse_version(std::string_view version, req *const req)
    {
        if (version.size() != version_len) {
            req->err = &bad_req_handler;
            return;
        }

        req->version.major = std::stoi(static_cast<std::string>(version.substr(version_major_start, 1)));
        req->version.minor = std::stoi(static_cast<std::string>(version.substr(version_minor_start, 1)));
    }

    void parse_field_line(std::string_view field_line, req *const req)
    {
        const size_t field_name_end = field_line.find(':');
        size_t field_value_start;
        std::string field_name;
        std::string_view field_value_view;

        if (field_name_end == std::string_view::npos) {
            goto err_response;
        }
        field_name = lowercase_str(field_line.substr(0, field_name_end));

        if (req->fields.contains(field_name)) {
            goto err_response;
        }

        field_value_start = field_line.find_first_not_of(space, field_name_end + 1); 
        if (field_value_start == std::string_view::npos) {
            goto err_response;
        }

        field_value_view = field_line.substr(field_value_start);
        req->fields[field_name] = field_value_view;

        if (field_name == host_header) {
            parse_field_host(field_value_view, req);
        }
        return;

    err_response:
        std::cout << "field error" << std::endl;
        req->err = &bad_req_handler;
    }

    void parse_field_host(std::string_view host_val, req *const req)
    {
        switch (req->target_form) {
            case http::origin:
                /* req_target contains path and query. */
                uri::parse_uri(&req->uri, host_val, uri::flag_scheme | uri::flag_authority);
                break;
            case http::authority:
                /* req_target contains host and port. */
                uri::parse_uri(&req->uri, host_val, ~uri::flag_authority);
                break;
            case http::asterik:
                /* req_target contains */
                uri::parse_uri(&req->uri, host_val, uri::flag_scheme | uri::flag_authority);
                break;
        }
    }

    /* The request headers have been parsed and validated by the time this is called.
    The content string view is not guaranteed to encompass all the
    content (the rx buffer may be smaller than rx data, transport receive may have
    notified us of rx packets before all where received.)*/
    void parse_content(size_t content_start_idx, req *const req)
    {
        parse_transfer_encoding(req);
        parse_content_len(req);

        if (req->has_err()) {
            return;
        }

        if (req->fields.contains(transfer_encoding)) {

            if (req->fields.contains(content_length)) {
                goto bad_req; // cannot have content-length and transfer-encoding.
            }

            while (!req->transfer_encodings.empty()) {
                /* if encoding was a polymorphic class, we could simplify the code here to:
                    encoding->decode();
                */
                encoding_t encoding = req->transfer_encodings.front();
                req->transfer_encodings.pop();

                switch (encoding) {
                    case gzip:
                        break;
                    case chunked:
                        /* chunked-size CRLF
                           chunked-data CRLF */
                        break;
                }
            }

        }
        else if (req->fields.contains(content_length)) {
            // for (auto it = content.begin(); it != std::min(content.begin() + req->content_len, content.end()); it++) 
        }
        req->parse_state = complete;
        return;
    
    bad_req:
        std::cout << "content error" << std::endl;
        req->err = &bad_req_handler;
    }

}
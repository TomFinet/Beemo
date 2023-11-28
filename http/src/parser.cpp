#include <http/parser.h>
#include <http/encoding.h>

#include <uri/uri_parser.h>

#include <unordered_map>


namespace
{

    constexpr auto space = ' ';
    constexpr auto crlf = "\r\n"; 

    constexpr int version_len = 8;
    constexpr int version_major_start = 5;
    constexpr int version_minor_start = 7;

    /* maps encoding_t to the corresponding encoding class. */
    const std::unordered_map<http::encoding_t, http::encoding::encoded*> encoding_map
    {
        {http::invalid_encoding, new http::encoding::encoded()},
        {http::chunked, new http::encoding::chunked()},
        {http::identity, new http::encoding::identity()},
        {http::gzip, new http::encoding::gzip()},
        {http::compress, new http::encoding::compress()},
        {http::deflate, new http::encoding::deflate()}
    };

    http::encoding::encoded* to_encoding_class(http::encoding_t enc)
    {
        if (!encoding_map.contains(enc)) {
            return encoding_map.at(http::invalid_encoding);
        }
        return encoding_map.at(enc);
    }

}


namespace http
{

    size_t parse_headers(std::string_view raw_req, struct req *const req, const struct parse_ctx &ctx)
    {
        std::string_view req_line;
        std::string_view field_line;
        size_t line_start = ctx.start_idx;
        size_t line_end;
        size_t req_line_end;
        size_t parsed_up_to = ctx.start_idx;

        if (req->parse_state == start_line) {
            req_line_end = raw_req.find(crlf);
            if (req_line_end == std::string_view::npos) {
                goto err_response;
            }

            req_line = {raw_req.begin(), raw_req.begin() + req_line_end}; 
            line_start = req_line_end + 2;
            parse_req_line(req_line, req, ctx.config->max_req_line_len); 
        }

        while (req->parse_state == parse_state_t::headers && !req->has_err()) {
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
    ret:
        return parsed_up_to;
   }

    void parse_req_line(std::string_view req_line, req *const req, const size_t max_req_line_len)
    {
        size_t method_end;
        size_t req_target_end;
        std::string_view req_target;
        std::string_view version;

        if (req_line.empty() && req_line.size() > max_req_line_len) {
            goto err_response;
        }

        method_end = req_line.find(space);
        if (method_end == std::string_view::npos) {
            goto err_response;
        }
        req->method = static_cast<method_t>(http_keyword_map::keyword_val(req_line.substr(0, method_end)));
        if (req->method == method_invalid) {
            goto err_not_impl;
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
        return;
    err_not_impl:
        req->err = &not_impl_handler;
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
        field_name = utils::lowercase_str(field_line.substr(0, field_name_end));

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
            parse_host(field_value_view, req);
        }
        return;

    err_response:
        req->err = &bad_req_handler;
    }

    void parse_host(std::string_view host_val, req *const req)
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
    
    void parse_content_len(struct req *const req)
    {
        if (!req->fields.contains(content_length_token) || req->fields.contains(transfer_encoding_token)) {
            return;
        }

        std::string_view content_len_view {req->fields[content_length_token]}; 
        bool is_valid = utils::parse_pattern(content_len_view.begin(), content_len_view.end(), [](unsigned char c)
        {
            return std::isdigit(c);
        });
        if (content_len_view.empty() || !is_valid) {
            req->err = &bad_req_handler;
            return;
        }

        std::string content_len {content_len_view};
        req->content_len = std::stoi(content_len);
    }

    /* content-type defines the media type and format used for the
    message's representation data (content, or whatever uri points to).
    It may also specify the character encoding used. */
    void parse_content_type(req *const req)
    {
        std::string_view content_type;
        std::string_view media_type; 
        std::string_view media_subtype;
        std::string_view charset;
        size_t media_type_end; 
        size_t media_subtype_end;

        if (!req->fields.contains(content_type_token)) {
            req->fields[content_type_token] = "application/json";
        }

        content_type = req->fields[content_type_token];

        media_type_end = content_type.find('/');
        media_type = content_type.substr(0, media_type_end);
        req->media_type = static_cast<media_type_t>(http_keyword_map::keyword_val(media_type));
        if (req->media_type == media_type_invalid) {
            req->err = &unsupported_media_type_handler;
            return;
        }

        media_subtype_end = content_type.find(';', media_type_end);
        media_subtype = content_type.substr(media_type_end + 1, media_subtype_end - media_type_end);
        req->media_subtype = static_cast<media_subtype_t>(http_keyword_map::keyword_val(media_subtype));
        if (req->media_subtype == media_subtype_invalid) {
            req->err = &unsupported_media_type_handler; /* should probably be an unimplemented response msg. */
            return;
        }

        if (media_subtype_end == std::string_view::npos) {
            return;
        }

        charset = content_type.substr(media_subtype_end + 1);
        /* TODO: parse charset_token parameter. Should have a generic parameter parsing method in utils which spits out the param name and value. */
    }

    void parse_connection(struct req *const req)
    {
        // parse the connection field. connection is kept alive by default, unless the connection: close is provided here.
    }

    /* The request headers have been parsed and validated by the time this is called.
    The content string view is not guaranteed to encompass all the
    content (the rx buffer may be smaller than rx data, transport receive may have
    notified us of rx packets before all where received.) */
    /* TODO: this may be called multiple times due partial requests. We don't want to parse header fields each time. */
    void parse_content(std::string_view raw_content, req *const req)
    {
        /* codings applied for transfer. */
        req->transfer_encodings = parse_field_list<encoding_t>(req, transfer_encoding_token, &not_impl_handler);
        /* codings listed here are characteristic of the representation. all other metadata about the represenation is about the coded form. */
        req->content_encodings = parse_field_list<encoding_t>(req, content_encoding_token, &bad_req_handler);

        parse_content_len(req);
        parse_content_type(req);

        if (req->has_err()) {
            return;
        }

        bool has_transfer_encodings = !req->transfer_encodings.empty();
        bool has_content_length = req->fields.contains(content_length_token);

        if (has_transfer_encodings) {
            if (has_content_length) {
                goto bad_req; /* cannot have content-length and transfer-encoding. */
            }

            /* TODO: would be more efficient to do this as we are parsing the transfer encodings. */
            size_t chunked_cnt = std::count(req->transfer_encodings.begin(), req->transfer_encodings.end(), chunked);
            if (chunked_cnt > 1 || (chunked_cnt == 1 && req->transfer_encodings.back() != chunked)) {
                goto bad_req;
            }

            for (encoding_t enc : req->transfer_encodings) {
                bool fully_decoded = to_encoding_class(enc)->decode(raw_content, req); 
                if (!fully_decoded) {
                    goto incomplete_content;
                }
            }
        }
        else if (has_content_length) {
            if (req->content_len > raw_content.size()) {
                goto incomplete_content;
            }
            req->content = raw_content.substr(0, req->content_len);
        }
        else if (req->method != get) {
            goto len_required;
        }
        
        for (encoding_t enc : req->content_encodings) {
            to_encoding_class(enc)->decode(req->content, req); 
        }

        req->parse_state = complete;

    incomplete_content:
        return;
    bad_req:
        req->err = &bad_req_handler;
        return;
    len_required:     
        req->err = &len_required_handler;
        return;
    }

}
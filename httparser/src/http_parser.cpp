#include <httparser/http_parser.h>

#include <uriparser/uri_error.h>

#include <iostream>

namespace http {

template<typename T>
void http_parser::push_and_clear(std::vector<T> &v, T &t)
{
    if (t.empty()) {
        return;
    }

    v.push_back(t);
    t.clear();
}

std::vector<line_t> http_parser::lex(std::string &raw_req)
{
    std::vector<line_t> req_line;
    std::vector<token_t> tokens;
    token_t token;
    
    raw_req += " \r";

    for (int i = 0; i < raw_req.length(); i++) {
        if (raw_req[i] == '\r') {
            push_and_clear<token_t>(tokens, token);
            push_and_clear<line_t>(req_line, tokens);
        } else if (std::isspace(raw_req[i])) {
            push_and_clear<token_t>(tokens, token);
        } else {
            token.push_back(raw_req[i]);
        }
    }

    return req_line;
}

void http_parser::parse(const std::vector<line_t> &lines)
{
    if (lines.empty()) {
        throw http_error("Request is empty.");
    }

    parse_req_line(lines[0]);
    for (int i = 1; i < lines.size() - 1; i++) {
        parse_field_line(lines[i]);
    }
    parse_body(lines[lines.size() - 1]);
}

void http_parser::parse_req_line(const line_t &req_line)
{
    if (req_line.size() != req_line_token_num) {
        throw http_error("Request line does not have the correct number of tokens.");
    }

    parse_method(req_line[0]);
    parse_req_target(req_line[1]);
    parse_version(req_line[2]);
}

void http_parser::parse_method(const token_t &method_val)
{
    req.method = static_cast<method_t>(keyword_val(method_val, "Unsupported HTTP method."));
}

void http_parser::parse_req_target(const token_t &req_target)
{
    if (req_target.at(0) == '*') {
        target_form = http::asterik;
        parse_asterik_form(req_target);
    }
    else if (req_target.at(0) == '/') {
        target_form = http::origin;
        parse_origin_form(req_target);
    }
    else if (req_target.find('/') != std::string::npos) {
        target_form = http::absolute;
        parse_absolute_form(req_target);
    }
    else {
        target_form = http::authority;
        parse_authority_form(req_target);
    }
}

void http_parser::parse_asterik_form(const token_t &req_target)
{
    if (req_target.length() > 1 || req_target.at(0) != '*') {
        req.uri.asterik = false;
        throw http_error("Invalid request target.");
    }
    req.uri.asterik = true; 
}

void http_parser::parse_origin_form(const token_t &req_target)
{
    req.uri.parse_uri(req_target, uri::flag_path | uri::flag_query);
}

void http_parser::parse_absolute_form(const token_t &req_target)
{
    req.uri.parse_uri(req_target, uri::flag_all);
}

void http_parser::parse_authority_form(const token_t &req_target)
{
    req.uri.parse_uri(req_target, uri::flag_authority);
}

void http_parser::parse_version(const token_t &version)
{
    int major = std::stoi(version.substr(version_index_start, version_index_start + 1));
    int minor = std::stoi(version.substr(version_index_start + 2, version_index_start + 3));

    req.version.major = major;
    req.version.minor = minor;
}

void http_parser::parse_field_line(const line_t &field)
{
    if (field.at(0) == connection_header) { 
        parse_header_connection(field.at(1));
    }
    else if (field.at(0) == encoding_header) { 
        parse_header_encoding(field.at(1));
    }
    else if (field.at(0) == host_header) {
        parse_header_host(field.at(1));
    }
}

void http_parser::parse_header_connection(const token_t &conn_val)
{
    req.conn_option = static_cast<conn_t>(keyword_val(conn_val, "Unknown connection field."));
}

void http_parser::parse_header_encoding(const token_t &encoding_val)
{
    req.encoding = static_cast<encoding_t>(keyword_val(encoding_val, "Unknown encoding field."));
}

void http_parser::parse_header_host(const token_t &host_val)
{
    switch (target_form) {
        case http::origin:
            /* req_target contains path and query. */
            req.uri.parse_uri(host_val, uri::flag_scheme | uri::flag_authority);
            break;
        case http::authority:
            /* req_target contains host and port. */
            req.uri.parse_uri(host_val, ~uri::flag_authority);
            break;
        case http::asterik:
            /* req_target contains */
            req.uri.parse_uri(host_val, uri::flag_scheme | uri::flag_authority);
            break;
    }

    if (req.uri.scheme == "") {
        req.uri.scheme = default_scheme;
    }
}

void http_parser::parse_body(const line_t &body_line)
{
    req.body = body_line[0];
}

}
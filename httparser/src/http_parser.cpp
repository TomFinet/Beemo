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
        throw parse_error("Request is empty.");
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
        throw parse_error("Request line does not have the correct number of tokens.");
    }

    parse_method(req_line[0]);
    parse_req_target(req_line[1]);
    parse_version(req_line[2]);
}

void http_parser::parse_method(const token_t &method_val)
{
    req.method = static_cast<method_t>(keyword_val(method_val, "Unknown http_parser method."));
}

void http_parser::parse_req_target(const token_t &req_target)
{
    if (req_target.at(0) == '*') {
        parse_asterik_form(req_target);
    }
    else if (req_target.at(0) == '/') {
        parse_origin_form(req_target);
    }
    else if (req_target.find('/') != std::string::npos) {
        target_form = http::absolute; /* the req_target is the uri */
        parse_absolute_form(req_target);
    }
    else {
        parse_authority_form(req_target);
    }
}

void http_parser::parse_asterik_form(const token_t &req_target)
{
    if (req_target.length() > 1 || req_target.at(0) != '*') {
        req.uri.asterik = false;
        throw parse_error("Invalid request target.");
    }
    req.uri.asterik = true; 
}

/* absolute-path [ "?" query ]*/
void http_parser::parse_origin_form(const token_t &req_target)
{
    size_t query_start_pos = req_target.find('?');
    if (query_start_pos == std::string::npos) {
        req.uri.parse_path_absolute(req_target.begin(), req_target.end());
        req.uri.query = "";
    }
    else {
        req.uri.parse_path_absolute(req_target.begin(), req_target.begin() + query_start_pos);
        req.uri.parse_query(req_target.begin() + query_start_pos, req_target.end());
    }
}

void http_parser::parse_absolute_form(const token_t &req_target)
{
    req.uri.parse_absolute_uri(req_target);
}

void http_parser::parse_authority_form(const token_t &req_target)
{
    size_t host_end_pos = req_target.find(':');
    if (host_end_pos == std::string::npos) {
        req.uri.parse_host(req_target.begin(), req_target.end());
    }
    else {
        req.uri.parse_host(req_target.begin(), req_target.begin() + host_end_pos);
        req.uri.parse_port(req_target.begin() + host_end_pos, req_target.end());
    }
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
        break;
        case http::authority:
        break;
        case http::asterik:
        break;
    }
}

void http_parser::parse_body(const line_t &body_line)
{
    req.body = body_line[0];
}

}
#include <algorithm>

#include <http/msg.h>

namespace http {

template<typename T>
void req::push_and_clear(std::vector<T> &v, T &t)
{
    if (t.empty()) {
        return;
    }

    v.push_back(t);
    t.clear();
}

std::vector<line_t> req::lex(std::string &raw_req)
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

void req::parse(const std::vector<line_t> &lines)
{
    if (lines.empty()) {
        throw error("Request is empty.");
    }

    parse_req_line(lines[0]);
    for (int i = 1; i < lines.size() - 1; i++) {
        parse_field_line(lines[i]);
    }
    parse_body(lines[lines.size() - 1]);
}

void req::parse_req_line(const line_t &req_line)
{
    if (req_line.size() != req_line_token_num) {
        throw error("Request line does not have the correct number of tokens.");
    }

    parse_method(req_line[0]);
    parse_req_target(req_line[1]);
    parse_version(req_line[2]);
}

void req::parse_method(const token_t &method_val)
{
    method = static_cast<method_t>(keyword_val(method_val, "Unsupported HTTP method."));
}

void req::parse_req_target(const token_t &req_target)
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

void req::parse_asterik_form(const token_t &req_target)
{
    if (req_target.length() > 1 || req_target.at(0) != '*') {
        uri.asterik = false;
        throw error("Invalid request target.");
    }
    uri.asterik = true; 
}

void req::parse_origin_form(const token_t &req_target)
{
    uri.parse_uri(req_target, uri::flag_path | uri::flag_query);
}

void req::parse_absolute_form(const token_t &req_target)
{
    uri.parse_uri(req_target, uri::flag_all);
}

void req::parse_authority_form(const token_t &req_target)
{
    uri.parse_uri(req_target, uri::flag_authority);
}

void req::parse_version(const token_t &vers)
{
    int major = std::stoi(vers.substr(version_index_start, version_index_start + 1));
    int minor = std::stoi(vers.substr(version_index_start + 2, version_index_start + 3));

    version.major = major;
    version.minor = minor;
}

/* we ignore any unrecognised field lines. */
void req::parse_field_line(const line_t &field)
{
    /* field[0] ends with ':', so we remove it. */
    std::string field_key(field.at(0).begin(), field.at(0).end() - 1);
    std::transform(field_key.begin(), field_key.end(), field_key.begin(), std::tolower);

    if (field_key == connection_header) { 
        field_map[connection_header] = static_cast<conn_t>(
            keyword_val(field.at(1), "Unknown connection field."));
    }
    else if (field_key == encoding_header) { 
        field_map[encoding_header] = static_cast<encoding_t>(
            keyword_val(field.at(1), "Unknown encoding field."));
    }
    else if (field_key == host_header) {
        parse_header_host(field.at(1));
    }
}

void req::parse_header_host(const token_t &host_val)
{
    switch (target_form) {
        case http::origin:
            /* req_target contains path and query. */
            uri.parse_uri(host_val, uri::flag_scheme | uri::flag_authority);
            break;
        case http::authority:
            /* req_target contains host and port. */
            uri.parse_uri(host_val, ~uri::flag_authority);
            break;
        case http::asterik:
            /* req_target contains */
            uri.parse_uri(host_val, uri::flag_scheme | uri::flag_authority);
            break;
    }

    if (uri.scheme == uri::no_scheme) {
        uri.scheme = default_scheme;
    }

    if (uri.port = uri::no_port) {
        uri.port = default_port;
    }
}

void req::parse_body(const line_t &body_line)
{
    body = body_line[0];
}

void req::validate()
{
    if (version.major != 1 || version.minor != 1) {
        throw error("Server only supports HTTP 1.1");
    }

    if (!field_map[host_header]) {
        throw error("Host header field cannot be missing.");
    }

    if (uri.scheme != default_scheme) {
        throw error("Unsupported scheme.");
    }

    if (uri.userinfo.empty()) {
        throw error("HTTP has depricated userinfo.");
    }
}


}
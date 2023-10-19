#include <regex>
#include <iostream>

#include <httparser/http_parser.h>


namespace httparser {

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

void http_parser::parse(const std::vector<line_t> &lines, http_req &req)
{
    if (lines.empty()) {
        throw parse_error("Request is empty.");
    }

    parse_req_line(lines[0], req);
    for (int i = 1; i < lines.size() - 1; i++) {
        parse_field_line(lines[i], req);
    }
    parse_body(lines[lines.size() - 1], req);
}

void http_parser::parse_req_line(const line_t &req_line, http_req &req)
{
    if (req_line.size() != req_line_token_num) {
        throw parse_error("Request line does not have the correct number of tokens.");
    }

    parse_method(req_line[0], req);
    parse_req_target(req_line[1], req);
    parse_version(req_line[2], req);
}

void http_parser::parse_method(const token_t &method_val, http_req &req)
{
    req.method = static_cast<method_t>(keyword_val(method_val, "Unknown HTTP method."));
}

void http_parser::parse_req_target(const token_t &req_target, http_req &req)
{
    std::regex reg(uri_regex);
    std::cout << std::regex_match(req_target, reg) << std::endl;
}

void http_parser::parse_version(const token_t &version, http_req &req)
{
    int major = std::stoi(version.substr(version_index_start, version_index_start + 1));
    int minor = std::stoi(version.substr(version_index_start + 2, version_index_start + 3));

    req.version.major = major;
    req.version.minor = minor;
}

void http_parser::parse_field_line(const line_t &field, http_req &req)
{
    if (field.at(0) == connection_header) { 
        parse_header_connection(field.at(1), req);
    } else if (field.at(0) == encoding_header) { 
        parse_header_encoding(field.at(1), req);
    } else if (field.at(0) == host_header) {
        parse_header_host(field.at(1), req);
    } else {
        throw parse_error("Unknown field.");
    }
}

void http_parser::parse_header_connection(const token_t &conn_val, http_req &req)
{
    req.conn_option = static_cast<conn_t>(keyword_val(conn_val, "Unknown connection field."));
}

void http_parser::parse_header_encoding(const token_t &encoding_val, http_req &req)
{
    req.encoding = static_cast<encoding_t>(keyword_val(encoding_val, "Unknown encoding field."));
}

void http_parser::parse_header_host(const token_t &host_val, http_req &req)
{
    token_t host;
    int port;

    int host_end = static_cast<int>(host_val.find(':'));
    if (host_end == std::string::npos) {
        host = host_val.substr(0, host_val.size());
        port = no_port;
    } else {
        host = host_val.substr(0, host_end);

        try {
            port = std::stoi(host_val.substr(host_end + 1, host_val.size()));
        } catch (std::invalid_argument const&) {
            throw parse_error("Unrecognised port number.");
        }
    }

    req.host.host = host;
    req.host.port = port;

}

void http_parser::parse_body(const line_t &body_line, http_req &req)
{
    req.body = body_line[0];
}

}
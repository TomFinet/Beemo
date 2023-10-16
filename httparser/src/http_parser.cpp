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
    parse_req_line(lines[0], req);
    for (int i = 1; i < lines.size() - 1; i++) {
        parse_header(lines[i], req);
    }
    parse_body(lines[lines.size() - 1], req);
}

void http_parser::parse_req_line(const line_t &req_line, http_req &req)
{
    parse_method(req_line[0], req);
    req.uri = req_line[1];
    req.version = req_line[2];
}

int http_parser::parse_method(const token_t &method_val, http_req &req)
{
    struct http_kvp *kvp = Perfect_Hash::in_word_set(method_val.c_str(),
        static_cast<unsigned int>(method_val.length()));
    if (kvp == NULL) {
        return 1;
    }
    req.method = static_cast<method_t>(kvp->value);
    return 0;
}

int http_parser::parse_header(const line_t &general_header, http_req &req)
{
    if (general_header[0].c_str() == connection_header) { 
        parse_header_connection(general_header[1], req);
    } else if (general_header[0].c_str() == encoding_header) { 
        parse_header_encoding(general_header[1], req);
    } else if (general_header[0] == host_header) {
        parse_header_host(general_header[1], req);
    } else {
        return 1;
    }
    return 0;
}

int http_parser::parse_header_connection(const token_t &conn_val, http_req &req)
{
    struct http_kvp *kvp = Perfect_Hash::in_word_set(conn_val.c_str(),
        static_cast<unsigned int>(conn_val.length()));
    if (kvp == NULL) {
        return 1;
    }
    req.conn_option = static_cast<conn_t>(kvp->value);
    return 0;
}

int http_parser::parse_header_encoding(const token_t &encoding_val, http_req &req)
{
    struct http_kvp *kvp = Perfect_Hash::in_word_set(encoding_val.c_str(),
        static_cast<unsigned int>(encoding_val.length()));
    if (kvp == NULL) {
        return 1;
    }
    req.encoding = static_cast<encoding_t>(kvp->value);
    return 0;
}

int http_parser::parse_header_host(const token_t &host_val, http_req &req)
{
    token_t host;
    int port;

    int host_end = static_cast<int>(host_val.find(':'));
    if (host_end == std::string::npos) {
        host = host_val.substr(0, host_val.size());
        port = -1;
    } else {
        host = host_val.substr(0, host_end);

        try {
            port = std::stoi(host_val.substr(host_end + 1, host_val.size()));
        } catch (std::invalid_argument const&) {
            port = -1;
        }
    }

    req.host.host = host;
    req.host.port = port;

    return 0;
}

int http_parser::parse_body(const line_t &body_line, http_req &req)
{
    req.body = body_line[0];
    return 0;
}

}
#include <http_parser.h>

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
        parse_header(line[i], req);
    }
    parse_body(line[lines.size() - 1], req);
}

void http_parser::parse_req_line(const line_t &req_line, http_req &req)
{
    req.method = req_line[0];
    req.uri = req_line[1];
    req.version = req_line[2];
}

void http_parser::parse_header(const line_t &general_header, http_req &req)
{

}

}
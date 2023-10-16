#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include <httparser/http_keyword_map.h>
#include <httparser/http_req.h>

namespace httparser {
    
typedef std::string token_t;
typedef std::vector<token_t> line_t;

/** what state does this need to track? if none then why is it a class. */
class http_parser {

    public:

        http_parser() {}
        ~http_parser() {}

        /**
         * Takes a raw http request and lexes it into tokenised request lines.
         *
         * @param raw_req The raw http request.
         *
         * @return The tokenised request lines. The request body is left as a raw string.
         */
        std::vector<line_t> lex(std::string &raw_req);

        /**
         * Parses a tokenised request into an @http_req object.
         *
         * @param req_lines Tokenised request lines. 
         * @param req the parsed HTTP request object.
         */
        void parse(const std::vector<line_t> &lines, http_req &req);

    private:

        template<typename T>
        void push_and_clear(std::vector<T> &v, T &t);

        void parse_req_line(const line_t &req_line, http_req &req);

        int parse_method(const token_t &method_val, http_req &req);

        int parse_header(const line_t &general_header, http_req &req);

        int parse_header_connection(const token_t &conn_val, http_req &req);

        int parse_header_encoding(const token_t &encoding_val, http_req &req);

        int parse_header_host(const token_t &host_val, http_req &req);

        int parse_body(const line_t &body_line, http_req &req);
};

}
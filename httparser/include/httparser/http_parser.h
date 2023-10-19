#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include <httparser/http_keyword_map.h>
#include <httparser/http_req.h>
#include <httparser/parse_error.h>

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
         * according to RFC 9112 sect 2.1.
         *
         * @param req_lines Tokenised request lines. 
         * @param req the parsed HTTP request object.
         */
        void parse(const std::vector<line_t> &lines, http_req &req);

    private:
        
        const static inline int req_line_token_num = 3;

        const static inline int version_index_start = 5;

        const static inline char uri_regex[] = 
            "^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?";


        /* RFC 9112 sect 3 */
        void parse_req_line(const line_t &req_line, http_req &req);

        /* RFC 9112 sect 3.1 */
        void parse_method(const token_t &method_val, http_req &req);

        /* RFC 9112 sect 3.2 */
        void parse_req_target(const token_t &req_target, http_req &req);

        /* RFC 9112 sect 2.3 */
        void parse_version(const token_t &version, http_req &req);
        
        /* RFC 9112 sect 3.0 */
        void parse_field_line(const line_t &field, http_req &req);

        void parse_header_connection(const token_t &conn_val, http_req &req);

        void parse_header_encoding(const token_t &encoding_val, http_req &req);

        void parse_header_host(const token_t &host_val, http_req &req);

        void parse_body(const line_t &body_line, http_req &req);

        inline int keyword_val(const token_t &keyword, const std::string &err_msg)
        {
            struct http_kvp *kvp = http_keyword_map::in_word_set(keyword.c_str(),
                static_cast<unsigned int>(keyword.length()));

            if (kvp == NULL) {
                throw parse_error(err_msg);
            }

            return kvp->value;
        }

        template<typename T>
        void push_and_clear(std::vector<T> &v, T &t);
};

}
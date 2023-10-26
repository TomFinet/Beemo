#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include <httparser/http_keyword_map.h>
#include <httparser/http_req.h>
#include <httparser/http_error.h>

#include <uriparser/uri.h>

namespace http {
    
typedef std::string token_t;
typedef std::vector<token_t> line_t;

enum target_form_t {
    absolute, authority, asterik, origin
};

class http_parser {

    public:

        target_form_t target_form;

        struct http_req req;

        http_parser() {}
        ~http_parser() {}

        std::vector<line_t> lex(std::string &raw_req);

        /**
         * Parses a tokenised request into an @http_req object.
         * according to RFC 9112 sect 2.1.
         *
         * @param req_lines Tokenised request lines. 
         * @param req the parsed HTTP request object.
         */
        void parse(const std::vector<line_t> &lines);

        /* RFC 9112 sect 3 */
        void parse_req_line(const line_t &req_line);

        /* RFC 9112 sect 3.1 */
        void parse_method(const token_t &method_val);

        /* RFC 9112 sect 3.2 */
        void parse_req_target(const token_t &req_target);
        void parse_asterik_form(const token_t &req_target);
        void parse_origin_form(const token_t &req_target);
        void parse_absolute_form(const token_t &req_target);
        void parse_authority_form(const token_t &req_target);

        /* RFC 9112 sect 2.3 */
        void parse_version(const token_t &version);
        
        /* RFC 9112 sect 3.0 */
        void parse_field_line(const line_t &field);

        void parse_header_connection(const token_t &conn_val);

        void parse_header_encoding(const token_t &encoding_val);

        void parse_header_host(const token_t &host_val);

        void parse_body(const line_t &body_line);

    private:

        const static inline int req_line_token_num = 3;

        const static inline int version_index_start = 5;

        const static inline char default_scheme[] = "http";

        inline unsigned int keyword_val(const token_t &keyword, const std::string &err_msg)
        {
            struct http_kvp *kvp = http_keyword_map::in_word_set(keyword.c_str(),
                static_cast<unsigned int>(keyword.length()));

            if (kvp == NULL) {
                throw http_error(err_msg);
            }

            return kvp->value;
        }

        template<typename T>
        void push_and_clear(std::vector<T> &v, T &t);
};

}
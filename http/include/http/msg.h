#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

#include <http/msg_constants.h>
#include <http/keyword_map.h>
#include <http/msg.h>
#include <http/error.h>

#include <uriparser/uri.h>

namespace http {

class msg {
    public:
        version_t version;
        std::unordered_map<std::string, int> field_map;
        std::string body;
};

class req : public msg {

    public:

        req() { }
        req(std::string &raw) { parse(lex(raw)); }
        ~req() { }

        method_t method;

        target_form_t target_form;
        uri::uri uri;   
        
        encoding_t encoding;
        content_type_t content_type;

        /**
         * List of client supported protocols it would like to switch to.
         * Server is free to ignore these upgrades.
         */
        std::vector<std::string> upgrades; 
        
        std::vector<line_t> lex(std::string &raw_req);

        /**
         * Parses a tokenised request into an @http_req object.
         * according to RFC 9112 sect 2.1.
         *
         * @param req_lines Tokenised request lines. 
         * @param req the parsed HTTP request object.
         */
        void parse(const std::vector<line_t> &lines);

        /* RFC 9112 sect status_code_len */
        void parse_req_line(const line_t &req_line);

        /* RFC 9112 sect status_code_len.1 */
        void parse_method(const token_t &method_val);

        /* RFC 9112 sect status_code_len.2 */
        void parse_req_target(const token_t &req_target);
        void parse_asterik_form(const token_t &req_target);
        void parse_origin_form(const token_t &req_target);
        void parse_absolute_form(const token_t &req_target);
        void parse_authority_form(const token_t &req_target);

        /* RFC 9112 sect 2.status_code_len */
        void parse_version(const token_t &version);
        
        /* RFC 9112 sect status_code_len.0 */
        void parse_field_line(const line_t &field);

        void parse_header_host(const token_t &host_val);

        void parse_body(const line_t &body_line);

        void validate(void);

    private:

        const static inline int req_line_token_num = 3;
        const static inline int version_index_start = 5;

        const static inline char default_scheme[] = "http";
        const static inline int default_port = 80;

        inline unsigned int keyword_val(const token_t &keyword, const std::string &err_msg)
        {
            struct http_kvp *kvp = http_keyword_map::in_word_set(keyword.c_str(),
                static_cast<unsigned int>(keyword.length()));

            if (kvp == NULL) {
                throw error(err_msg);
            }

            return kvp->value;
        }

        template<typename T>
        static void push_and_clear(std::vector<T> &v, T &t);

};

class response : public msg {
    char status_code[status_code_len];
    char *reason; 
};

}
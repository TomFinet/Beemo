#pragma once

#include <string>
#include <unordered_map>
#include <queue>
#include <sstream>

#include <uri/uri.h>

namespace beemo
{
    namespace
    {
        constexpr auto &host_header_token = "host";
        constexpr auto &content_length_token = "content-length";
        constexpr auto &transfer_encoding_token = "transfer-encoding";
        constexpr auto &content_type_token = "content-type";
        constexpr auto &content_encoding_token = "content-encoding";
        constexpr auto &charset_token = "charset";
    }

    struct version {
        short major;
        short minor;
    };

    enum method_t {
        method_invalid,
        get, post, options, head, put,
        del, trace, connect, extension
    };

    enum conn_t { conn_invalid, keep_alive, close };

    enum media_type_t { media_type_invalid, application, text };
    enum media_subtype_t { media_subtype_invalid, html, json, x_www_form_urlencoded, octet_stream };
    enum encoding_t { invalid_encoding, chunked, identity, gzip, compress, deflate };

    enum charset_t { charset_invalid, utf8 };

    enum target_form_t { absolute, authority, asterik, origin };
    
    /* Current parsing stage. */
    enum parse_state_t { start_line, headers, content, chunk_trailers, complete };

    /* Break circular dependency. */
    class err_builder;

    struct req {
        req() : parse_state(start_line), err(nullptr), content_len(0), chunked_content_len(0) { }

        /* Metadata to aid with request parsing. */
        target_form_t target_form;
        parse_state_t parse_state;
        err_builder *err;

        struct version version;
        method_t method;
        uri::uri uri;
        std::unordered_map<std::string, std::string> fields;

        /* When content-length is specified in req, this takes its value.
        If we use chunked encoding, this keeps track of the number of bytes
        of content we have decoded so far. */
        unsigned int chunked_content_len;
        unsigned int content_len;
        media_type_t media_type;
        media_subtype_t media_subtype;
        charset_t charset;

        std::vector<encoding_t> content_encodings;
        std::vector<encoding_t> transfer_encodings;

        std::string content;

        bool has_err(void) const;
        bool is_parsing_headers(void) const;
        bool is_parsing_incomplete(void) const;
    };

    constexpr int status_code_len = 4;

    struct resp {

        struct version version;
        unsigned short status_code;
        std::string reason; 
        std::unordered_map<std::string, std::string> fields;
        std::string content;

        resp() : version{1, 1} { }
        resp(unsigned short status_code, const std::string &reason)
            : version{1, 1}, status_code(status_code), reason(reason) { }
        ~resp() = default;

        void put_header(const std::string &header, const std::string &value);
        std::string to_str(void);
    };
}
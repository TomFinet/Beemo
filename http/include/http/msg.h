#pragma once

#include <string>
#include <unordered_map>
#include <queue>
#include <sstream>

#include <http/headers.h>

#include <uri/uri.h>

namespace http
{

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
    
    /* Indicates what stage of message parsing we are at. */
    enum parse_state_t { start_line, headers, content, chunk_trailers, complete };

    /* forward declarations to break circular dependency. */
    class err_handler;

    /* A request is not necessarily fully sent in a single transport packet, we need to account for this.
    If we have not hit the double CLRF yet, then we are still reading header fields.*/
    struct req {

        req() : parse_state(start_line), err(nullptr), content_len(0), chunked_content_len(0) { }

        /* metadata to aid with request parsing. */
        target_form_t target_form;
        parse_state_t parse_state;
        err_handler *err;

        /* request line */
        struct version version;
        method_t method;
        uri::uri uri;

        /* fields */
        std::unordered_map<std::string, std::string> fields;

        /* parsed fields */
        /* when content-length is specified in req, this takes its value.
        if we use chunked encoding, this keeps track of the number of bytes of content
        we have decoded so far. */
        unsigned int chunked_content_len;
        unsigned int content_len;
        media_type_t media_type;
        media_subtype_t media_subtype;
        charset_t charset;

        std::vector<encoding_t> content_encodings;
        std::vector<encoding_t> transfer_encodings;

        /* body */
        std::string content;

        void print(void) const;
        bool has_err(void) const;
    };

    constexpr int status_code_len = 4;

    struct response {

        /* metadata */

        /* status line */
        struct version version;
        unsigned short status_code;
        std::string reason; 

        /* fields */
        std::unordered_map<std::string, std::string> fields;

        std::string content;

        response() : version{1, 1} { }
        response(unsigned short status_code, const std::string &reason)
            : version{1, 1}, status_code(status_code), reason(reason) { }


        inline void add_header(const std::string &header, const std::string &value)
        {
            fields[header] = value;
        }

        std::string to_str(void)
        {
            std::stringstream ss;
            ss << "HTTP/" << version.major << "." << version.minor << " " << status_code << " " << reason << "\r\n";

            for (auto &field : fields) {
                ss << field.first << ": " << field.second << "\r\n";
            }

            if (!content.empty()) {
                ss << "\r\n" << content;
            }

            return ss.str();
        }
    };

}
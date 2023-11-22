#pragma once

#include <string>
#include <unordered_map>
#include <queue>

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

    constexpr auto &host_header = "host";

    constexpr auto &ok = "200";
    constexpr auto &created = "201";
    constexpr auto &accepted = "202";
    constexpr auto &no_content = "204";

    constexpr auto &multi_choices = "300";
    constexpr auto &moved_permanently = "301";
    constexpr auto &found = "302";

    //constexpr auto &bad_req = "400";
    constexpr auto &unauthorised = "401";
    constexpr auto &forbidden = "403";
    constexpr auto &not_found = "404";
    constexpr auto &req_timeout = "408";
    constexpr auto &unsupported_media_type = "415";
    constexpr auto &upgrade_required = "426";

    constexpr auto &internal_err = "500";
    constexpr auto &not_implemented = "501";
    constexpr auto &bad_gateway = "502";
    constexpr auto &service_unavailable = "503";

    constexpr auto &content_length_token = "content-length";
    constexpr auto &transfer_encoding_token = "transfer-encoding";
    constexpr auto &content_type_token = "content-type";
    constexpr auto &content_encoding_token = "content-encoding";
    constexpr auto &charset_token = "charset";

    constexpr int status_code_len = 4;
    
    /* this is a server default and not a message constant, should be moved to server. */
    constexpr auto &default_content_type = "application/json";

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

    };

}
#pragma once

#include <string>
#include <unordered_map>
#include <queue>

#include <uri/uri.h>

namespace http {

    /* RFC 9112 sect 2.3 */
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

    enum encoding_t { encoding_invalid, chunked, identity, gzip, compress, deflate };

    enum content_type_t { content_type_invalid, html, json };

    enum target_form_t { absolute, authority, asterik, origin };
    
    /* Indicates what stage of message parsing we are at. */
    enum parse_state_t { start_line, headers, content, complete };

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
    
    /* forward declaration to break circular dependency. */
    class err_handler;

    /* A request is not necessarily fully sent in a single TCP, we need to account for this.
    If we have not hit the double CLRF yet, then we are still reading header fields.*/
    struct req {

        req() : parse_state(start_line), err(nullptr), content_len(0) { }

        /* metadata */
        target_form_t target_form;
        err_handler *err;
        parse_state_t parse_state;

        /* request line */
        struct version version;
        method_t method;
        uri::uri uri;

        /* fields */
        std::unordered_map<std::string, std::string> fields;

        /* parsed fields */
        unsigned int content_len;
        /* in order of preference. */
        std::queue<encoding_t> transfer_encodings;

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

    };

}
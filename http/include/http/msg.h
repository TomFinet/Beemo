#pragma once

#include <string>
#include <unordered_map>

#include <uri/uri.h>

namespace http {

    /* RFC 9112 sect 2.3 */
    struct version {
        short major;
        short minor;
    };

    enum method_t {
        get, post, options, head, put,
        del, trace, connect, extension 
    };

    enum conn_t {
        keep_alive, close 
    };

    enum encoding_t {
        chunked, identity, gzip, compress, deflate 
    };

    enum content_type_t {
        html, json
    };

    enum target_form_t {
        absolute, authority, asterik, origin
    };

    enum status_t {
        unparsed, headers, completed
    };

    constexpr auto &connection_header = "connection";
    constexpr auto &encoding_header = "transfer-encoding";
    constexpr auto &host_header = "host";
    constexpr auto &content_type_header = "content-type";

    constexpr auto &ok = "200";
    constexpr auto &created = "201";
    constexpr auto &accepted = "202";
    constexpr auto &no_content = "204";

    constexpr auto &multi_choices = "300";
    constexpr auto &moved_permanently = "301";
    constexpr auto &found = "302";

    constexpr auto &bad_req = "400";
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

    struct req {

        /* metadata */
        target_form_t target_form;
        status_t status;
        bool valid = true;

        /* request line */
        struct version version;
        method_t method;
        uri::uri uri;

        /* fields */
        std::unordered_map<std::string, std::string> fields;

        /* body */
        std::string body;

        void print(void) const;
    };

    constexpr int status_code_len = 4;

    class response {

        /* metadata */

        /* status line */
        struct version version;
        char status_code[status_code_len];
        std::string reason; 

        /* fields */
        std::unordered_map<std::string, std::string> fields;

        /* body */
        std::string body;
    };

}
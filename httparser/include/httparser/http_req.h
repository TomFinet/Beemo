#pragma once

#include <string>
#include <vector>

#include <uriparser/uri.h>

namespace http {

constexpr auto &connection_header = "Connection:";
constexpr auto &encoding_header = "Transfer-Encoding:";
constexpr auto &host_header = "Host:";
constexpr auto &content_type_header = "Content-Type:";

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

/* RFC 9112 sect 2.3 */
struct version_t {
    short major;
    short minor;
};

const int no_port = -1;


struct http_req {

        method_t method;

        /**
         * May be an absolute uri, absolute path, authority,
         * or "*" meaning the request is not resource specific.
         */
        uri::uri uri;   

        /**
         * The http version.
         */
        version_t version;

        /**
         * Optional connection setting. For example, the "close" option
         * signals the connection will be closed on response completion. 
         */
        conn_t conn_option;         

        /**
         * The encoding applied to the http request body. 
         */
        encoding_t encoding;

        /**
         * List of client supported protocols it would like to switch to.
         * Server is free to ignore these upgrades.
         */
        std::vector<std::string> upgrades; 

        content_type_t content_type;

        /**
         * The actual request body.
         */
        std::string body;
};

}
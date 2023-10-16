#pragma once

#include <string>
#include <vector>

#include <httparser/http_keyword_map.h>

namespace httparser {

constexpr auto &connection_header = "Connection:";
constexpr auto &encoding_header = "Transfer-Encoding:";
constexpr auto &host_header = "Host:";
constexpr auto &content_type_header = "Content-Type:";

/**
 * Refer to section 14.23 of RFC2616.
 */
struct host_t {
    std::string host;
    int port;
};

/**
 * Refer to section 5 of RFC2616.
 * 
 * Assumes that accept req header is JSON.
 */
struct http_req {

        method_t method;

        /**
         * May be an absolute uri, absolute path, authority,
         * or "*" meaning the request is not resource specific.
         */
        std::string uri;   

        /**
         * The http version.
         */
        std::string version;

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

        /**
         * Naming authority of the origin server.
         * Allows the origin server to differentiate between
         * internally ambiguous URLs.
         * Missing port information falls back to default.
         */
        host_t host;

        content_type_t content_type;

        /**
         * The actual request body.
         */
        std::string body;
};

}
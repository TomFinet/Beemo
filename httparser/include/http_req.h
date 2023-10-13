#pragma once

#include <string>
#include <vector>

namespace httparser {

/* All allowed HTTP request methods. */
namespace method {
    constexpr auto& GET = "GET"; 
    constexpr auto& POST = "POST";
    constexpr auto& OPTIONS = "OPTIONS";
    constexpr auto& HEAD = "HEAD";
    constexpr auto& PUT = "PUT";
    constexpr auto& DELETE = "DELETE";
    constexpr auto& TRACE = "TRACE";
    constexpr auto& CONNECT = "CONNECT";
    constexpr auto& EXTENSION = "EXTENSION";
}

enum encoding_t {
    CHUNKED, IDENTITY, GZIP, COMPRESS, DEFLATE
};

enum conn_t {
    KEEP_ALIVE, CLOSE
};

/**
 * The protocols supported by this server.
 *
 * TODO: this should not be in parser. Really parser should be able to handle
 * any http request. Thus supported protocols is a limitation/concern of the server.
 */
//const char* supported_protocols[] = {"HTTP/1.1", "Websockets"}; 

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

        std::string method;

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
         * Optional connection options. For example, the "close" option
         * signals the connection will be closed on response completion. 
         */
        std::vector<conn_t> conn_options;         

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

        /**
         * The actual request body.
         */
        std::string body;
};

}
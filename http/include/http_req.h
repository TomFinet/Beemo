#pragma once

#include <string>
#include <vector>

namespace httparser {

/* All allowed HTTP request methods. */
enum http_method {
    OPTIONS, GET, HEAD, POST, PUT,
    DELETE, TRACE, CONNECT, EXTENSION
};

enum body_encoding {
    CHUNKED, IDENTITY, GZIP, COMPRESS, DEFLATE
};

class http_req {

    private:

        http_method method;

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
        std::vector<std::string> conn_options;         

        /**
         * The encoding applied to the http request body. 
         */
        body_encoding encoding;

        
};

}
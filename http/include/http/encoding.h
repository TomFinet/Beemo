#pragma once

#include <string_view>

#include <http/msg.h>


namespace http
{

    namespace encoding
    {

        /* TODO: since encodings will only every have two functions (encode, decode).
        It is likely better to use inline function pointers to the functions instead of virtual tables.
        Furthermore, these classes are singletons, and so we have one instance with 2 virtual functions. */
        class encoded {
            public:
                /* returns true or false depending on if decoding has completed or not. */
                virtual bool decode(std::string_view raw_content, req *const request) const;
        };

        class chunked : public encoded {
            public:
                bool decode(std::string_view raw_content, req *const request) const override;
        };

        class identity : public encoded {
            public:
                bool decode(std::string_view content, req *const request) const override;
        };

        class compress : public encoded {
            public:
                bool decode(std::string_view content, req *const request) const override;
        };

        class deflate : public encoded {
            public:
                bool decode(std::string_view content, req *const request) const override;
        };

        class gzip : public encoded {
            public:
                bool decode(std::string_view content, req *const request) const override;
        };

    }
}
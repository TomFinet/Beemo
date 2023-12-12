#pragma once

#include <string_view>
#include <functional>

#include <http/msg.h>


namespace http
{
    using decoder_func_t = std::function<bool(std::string_view, req *const)>;

    namespace encoding
    {
        bool decode(std::string_view raw_content, req *const request);

        namespace chunked
        {
            bool decode(std::string_view raw_content, req *const request);
        }

        namespace identity
        {
            bool decode(std::string_view content, req *const request);
        }

        namespace compress
        {
            bool decode(std::string_view content, req *const request);
        }

        namespace deflate
        {
            bool decode(std::string_view content, req *const request);
        }

        namespace gzip
        {
            bool decode(std::string_view content, req *const request);
        }
    }
}
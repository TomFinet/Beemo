#pragma once

#include <http/msg.h>

#include <string_view>
#include <functional>


namespace beemo
{
    using decoder_t = std::function<bool(std::string_view, req *const)>;

    bool decode_chunked(std::string_view raw_content, req *const request);
    bool decode_identity(std::string_view content, req *const request);
    bool decode_compress(std::string_view content, req *const request);
    bool decode_deflate(std::string_view content, req *const request);
    bool decode_gzip(std::string_view content, req *const request);
}
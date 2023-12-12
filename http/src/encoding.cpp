#include <http/encoding.h>
#include <http/parser.h>

#include <sstream>
#include <iostream>


namespace http
{

    namespace encoding
    {

        bool decode(std::string_view raw_content, req *const request)
        {
            return true;
        }

        /* the raw_content is transferred in chunks. Each with the following structure.
            chunk-size [chunk-ext] CRLF
            chunk-data CRLF

            raw_content encompasses all the content, including potentially
            previously decoded content.
        */
        namespace chunked
        {
            bool decode(std::string_view raw_content, req *const request)
            {
                unsigned int chunk_size;

                if (request->parse_state == content) {
                    do {
                        chunk_size = 0;
                        size_t chunk_size_end = raw_content.find(' ', request->chunked_content_len);
                        size_t chunk_header_end = raw_content.find("\r\n", request->chunked_content_len);

                        if (chunk_size_end == std::string_view::npos) {
                            chunk_size_end = chunk_header_end;
                        }

                        if (chunk_size_end == std::string_view::npos) {
                            /* chunk header incomplete, ask for rx. */
                            return false;
                        }

                        std::stringstream ss;
                        ss << std::hex << raw_content.substr(request->chunked_content_len, chunk_size_end - request->chunked_content_len);
                        ss >> chunk_size;

                        /* if the chunk_size is greater than the raw_content string, then we must rx more data from connection. */
                        if (chunk_size > raw_content.size() - chunk_header_end - 2) {
                            return false;
                        }

                        std::string chunk_str {raw_content.substr(chunk_header_end + 2, chunk_size)};
                        request->content += chunk_str;
                        request->chunked_content_len = static_cast<unsigned int>(chunk_header_end) + 2 + chunk_size + 2;
                        request->content_len += chunk_size;
                        
                    } while (chunk_size > 0);
                }

                request->parse_state = chunk_trailers;

                /* check if no trailers */
                if (request->chunked_content_len == raw_content.size()) {
                    return true;
                }

                /* now we read the trailer section which has structure *(field-line CRLF). */
                size_t field_line_start = request->chunked_content_len + 1;
                while (field_line_start < raw_content.size()) {
                    size_t field_line_end = raw_content.find("\r\n", field_line_start);

                    if (field_line_end == std::string_view::npos) {
                        /* field line incomplete, ask for rx. */
                        return false;
                    }

                    if (field_line_start == field_line_end) {
                        return true; /* done. */
                    }

                    parse_field_line(raw_content.substr(field_line_start, field_line_end - field_line_start), request);

                    field_line_start = field_line_end + 2;
                }
                return false;
            }
        }

        namespace identity
        { 
            bool decode(std::string_view raw_content, req *const request)
            {
                return true;
            }
        }
        
        namespace gzip
        {
            bool decode(std::string_view raw_content, req *const request)
            {
                return true;
            }
        }

        namespace compress
        {
            bool decode(std::string_view raw_content, req *const request)
            {
                return true;
            }
        }
        
        namespace deflate
        {
            bool decode(std::string_view raw_content, req *const request)
            {
                return true;
            }
        }
    }
}
#include <http/http_encoding.h>
#include <http/http_parser.h>

#include <sstream>
#include <iostream>


namespace http
{

    namespace encoding
    {

        bool encoded::decode(std::string_view raw_content, req *const request) const
        {
            return true;
        }

        /* the raw_content is transferred in chunks. Each with the following structure.
            chunk-size [chunk-ext] CRLF
            chunk-data CRLF

            raw_content encompasses all the content, including potentially
            previously decoded content.
        */
        bool chunked::decode(std::string_view raw_content, req *const request) const
        {
            std::cout << "decoding: " << raw_content << std::endl;

            unsigned int chunk_size = 0;

            if (request->parse_state == content) {
                do {
                    size_t chunk_size_end = raw_content.find(' ', request->content_len);
                    size_t chunk_header_end = raw_content.find("\r\n", request->content_len);

                    if (chunk_size_end == std::string_view::npos) {
                        chunk_size_end = chunk_header_end;
                    }

                    if (chunk_size_end == std::string_view::npos) {
                        /* chunk header incomplete, ask for rx. */
                        return false;
                    }

                    std::stringstream ss;
                    ss << std::hex << raw_content.substr(request->content_len, request->content_len + chunk_size_end);
                    ss >> chunk_size;

                    /* if the chunk_size is greater than the raw_content string, then we must rx more data from connection. */
                    if (chunk_size > raw_content.size() - chunk_size_end) {
                        return false;
                    }

                    request->content += raw_content.substr(request->content_len + chunk_header_end + 2, chunk_size);
                    request->content_len += chunk_size;
                    
                } while (chunk_size > 0);
            }

            request->parse_state = chunk_trailers;

            /* now we read the trailer section which has structure *(field-line CRLF). */
            size_t field_line_start = request->content_len + 1;
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
}
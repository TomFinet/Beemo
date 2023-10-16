#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>

#include <httparser/http_parser.h>
#include <httparser/http_req.h>
#include <httparser/http_keyword_map.h>

TEST(ParserTest, Parse_Get_Valid) {
    std::vector<httparser::line_t> get = {
        {"GET", "http://www.google.com", "HTTP/1.1"},
        {"Connection:", "keep-alive"},
        {"Transfer-Encoding:", "chunked"},
        {"Test", "body..."}
    };

    httparser::http_parser parser;
    httparser::http_req req;
    parser.parse(get, req);

    ASSERT_EQ(req.method, httparser::GET);
    ASSERT_EQ(req.uri, get[0][1]);
    ASSERT_EQ(req.version, get[0][2]);
    ASSERT_EQ(req.encoding, httparser::CHUNKED);
}
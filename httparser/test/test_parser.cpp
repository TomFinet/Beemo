#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>

#include <http_parser.h>
#include <http_req.h>

TEST(ParserTest, Parse_Get_Valid) {
    std::vector<httparser::line_t> get = {
        {"GET", "http://www.google.com", "HTTP/1.1"},
        {"Connection:", "keep-alive"},
        {"Test", "body..."}
    };

    httparser::http_parser parser;
    httparser::http_req req;
    parser.parse(get, req);

    ASSERT_EQ(req.method, get[0][0]);
    ASSERT_EQ(req.uri, get[0][1]);
    ASSERT_EQ(req.version, get[0][2]);
}
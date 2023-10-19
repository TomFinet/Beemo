#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>

#include <httparser/http_parser.h>
#include <httparser/http_req.h>
#include <httparser/http_keyword_map.h>

TEST(ParserTest, Parse_Get_Valid) {
    std::vector<httparser::line_t> get = {
        {"GET", "/search?q=warhammer", "HTTP/1.1"},
        {"Connection:", "keep-alive"},
        {"Transfer-Encoding:", "chunked"},
        {"Host:", "www.google.com"},
        {"Test body..."}
    };

    httparser::http_parser parser;
    httparser::http_req req;
    parser.parse(get, req);

    ASSERT_EQ(req.method, httparser::get);
    ASSERT_EQ(req.uri, get[0][1]);
    ASSERT_EQ(req.version.major, 1);
    ASSERT_EQ(req.version.minor, 1);
    ASSERT_EQ(req.encoding, httparser::chunked);
    ASSERT_EQ(req.conn_option, httparser::keep_alive);
    ASSERT_EQ(req.host.host, get[3][1]);
    ASSERT_EQ(req.host.port, httparser::no_port);
    ASSERT_EQ(req.body, get[4][0]);
}
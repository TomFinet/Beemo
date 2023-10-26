#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>

#include <httparser/http_error.h>
#include <httparser/http_parser.h>
#include <httparser/http_req.h>
#include <httparser/http_keyword_map.h>

TEST(HttpTest, Parse_Get_Valid)
{
    std::vector<http::line_t> get = {
        {"GET", "/search?q=warhammer", "HTTP/1.1"},
        {"Connection:", "keep-alive"},
        {"Transfer-Encoding:", "chunked"},
        {"Host:", "www.google.com"},
        {"Test body..."}
    };

    http::http_parser parser;
    parser.parse(get);

    ASSERT_EQ(parser.req.method, http::get);
    ASSERT_EQ(parser.req.uri.path, "/search");
    ASSERT_EQ(parser.req.uri.query, "?q=warhammer");
    ASSERT_EQ(parser.req.version.major, 1);
    ASSERT_EQ(parser.req.version.minor, 1);
    ASSERT_EQ(parser.req.encoding, http::chunked);
    ASSERT_EQ(parser.req.conn_option, http::keep_alive);
    ASSERT_EQ(parser.req.body, get[4][0]);
}

TEST(HttpTest, AsterikForm)
{
    http::http_parser parser;
    parser.parse_asterik_form("*");
    ASSERT_TRUE(parser.req.uri.asterik);

    ASSERT_THROW(parser.parse_asterik_form("*/"), http::http_error);
    ASSERT_FALSE(parser.req.uri.asterik);
}

TEST(HttpTest, OriginForm)
{
    http::http_parser parser;
    std::string abspath;
    
    abspath = "/abs/pa1h";
    parser.parse_origin_form(abspath);
    ASSERT_EQ(parser.req.uri.path, abspath);
    ASSERT_EQ(parser.req.uri.query, "");

    abspath = "/abs/pa1h?q=val";
    parser.parse_origin_form(abspath);
    ASSERT_EQ(parser.req.uri.path, "/abs/pa1h");
    ASSERT_EQ(parser.req.uri.query, "?q=val");
}

TEST(HttpTest, AuthorityForm)
{  
    http::http_parser parser;
    std::string path;
    
    path = "www.google.com:8080/abs/pa1h?search=waffle";
    parser.parse_authority_form(path);
    ASSERT_EQ(parser.req.uri.reg_name, "www.google.com");
    ASSERT_EQ(parser.req.uri.port, 8080);
}
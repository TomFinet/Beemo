#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>

#include <http/error.h>
#include <http/msg.h>
#include <http/msg_constants.h>

TEST(HttpTest, Parse_Get_Valid)
{
    std::vector<http::line_t> get = {
        {"GET", "/search?q=warhammer", "HTTP/1.1"},
        {"Connection:", "keep-alive"},
        {"Transfer-Encoding:", "chunked"},
        {"Host:", "www.google.com"},
        {"Test body..."}
    };

    http::req req;
    req.parse(get);

    ASSERT_EQ(req.method, http::get);
    ASSERT_EQ(req.uri.path, "/search");
    ASSERT_EQ(req.uri.query, "?q=warhammer");
    ASSERT_EQ(req.version.major, 1);
    ASSERT_EQ(req.version.minor, 1);
    ASSERT_EQ(req.encoding, http::chunked);
    ASSERT_EQ(req.field_map[http::connection_header], http::keep_alive);
    ASSERT_EQ(req.body, get[4][0]);
}

TEST(HttpTest, AsterikForm)
{
    http::req req;
    req.parse_asterik_form("*");
    ASSERT_TRUE(req.uri.asterik);

    ASSERT_THROW(req.parse_asterik_form("*/"), http::error);
    ASSERT_FALSE(req.uri.asterik);
}

TEST(HttpTest, OriginForm)
{
    http::req req;
    std::string abspath;
    
    abspath = "/abs/pa1h";
    req.parse_origin_form(abspath);
    ASSERT_EQ(req.uri.path, abspath);
    ASSERT_EQ(req.uri.query, "");

    abspath = "/abs/pa1h?q=val";
    req.parse_origin_form(abspath);
    ASSERT_EQ(req.uri.path, "/abs/pa1h");
    ASSERT_EQ(req.uri.query, "?q=val");
}

TEST(HttpTest, AuthorityForm)
{  
    http::req req;
    std::string path;
    
    path = "www.google.com:8080/abs/pa1h?search=waffle";
    req.parse_authority_form(path);
    ASSERT_EQ(req.uri.reg_name, "www.google.com");
    ASSERT_EQ(req.uri.port, 8080);
}
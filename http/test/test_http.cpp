#include <gtest/gtest.h>

#include <string_view>
#include <stdexcept>

#include <http/msg.h>
#include <http/req_parser.h>

TEST(HttpTest, Parse_Get_Valid)
{
    std::string_view get =
        "GET /search?q=warhammer HTTP/1.1\r\n"
        "Connection: keep-alive\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Host: www.google.com\r\n"
        "\r\n"
        "Test body...";

    http::req req;
    http::req_parser parser(&req);
    parser.parse(get);

    ASSERT_EQ(req.method, http::get);
    ASSERT_EQ(req.uri.path, "/search");
    ASSERT_EQ(req.uri.query, "?q=warhammer");
    ASSERT_EQ(req.version.major, 1);
    ASSERT_EQ(req.version.minor, 1);
    ASSERT_EQ(req.fields[http::encoding_header], "chunked");
    ASSERT_EQ(req.fields[http::connection_header], "keep-alive");
    ASSERT_EQ(req.body, "Test body...");
}

TEST(HttpTest, AsterikForm)
{
    http::req req;
    http::req_parser parser(&req);

    parser.parse_req_target("*");
    ASSERT_TRUE(req.uri.asterik);

    ASSERT_THROW(parser.parse_req_target("*/"), std::domain_error);
    ASSERT_FALSE(req.uri.asterik);
}

TEST(HttpTest, OriginForm)
{
    http::req req;
    http::req_parser parser(&req);
    std::string_view abspath;
    
    abspath = "/abs/pa1h";
    parser.parse_req_target(abspath);
    ASSERT_EQ(req.uri.path, abspath);
    ASSERT_EQ(req.uri.query, "");

    abspath = "/abs/pa1h?q=val";
    parser.parse_req_target(abspath);
    ASSERT_EQ(req.uri.path, "/abs/pa1h");
    ASSERT_EQ(req.uri.query, "?q=val");
}

TEST(HttpTest, AuthorityForm)
{  
    http::req req;
    http::req_parser parser(&req);
    std::string_view path;
    
    path = "www.google.com:8080/abs/pa1h?search=waffle";
    parser.parse_req_target(path);
    ASSERT_EQ(req.uri.reg_name, "www.google.com");
    ASSERT_EQ(req.uri.port, 8080);
}
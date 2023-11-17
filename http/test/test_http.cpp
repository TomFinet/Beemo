#include <gtest/gtest.h>

#include <string_view>
#include <stdexcept>
#include <memory>

#include <http/msg.h>
#include <http/http_parser.h>

TEST(HttpTest, Parse_Valid_Headers)
{
    std::string_view get =
        "GET /search?q=warhammer HTTP/1.1\r\n"
        "Connection: keep-alive\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Host: www.google.com\r\n"
        "\r\n"
        "Test body...";

    std::unique_ptr<http::req> req = std::make_unique<http::req>();
    http::parse_headers(get, 0, req.get());

    ASSERT_EQ(req->method, http::get);
    ASSERT_EQ(req->uri.path, "/search");
    ASSERT_EQ(req->uri.query, "?q=warhammer");
    ASSERT_EQ(req->version.major, 1);
    ASSERT_EQ(req->version.minor, 1);
    ASSERT_EQ(req->fields["transfer-encoding"], "chunked");
    ASSERT_EQ(req->fields["connection"], "keep-alive");
}

TEST(HttpTest, AsterikForm)
{
    http::req req;

    http::parse_req_target("*", &req);
    ASSERT_TRUE(req.uri.asterik);

    http::parse_req_target("*/", &req);
    ASSERT_NE(req.err, nullptr);
    ASSERT_FALSE(req.uri.asterik);
}

TEST(HttpTest, OriginForm)
{
    http::req req;
    std::string_view abspath;
    
    abspath = "/abs/pa1h";
    http::parse_req_target(abspath, &req);
    ASSERT_EQ(req.uri.path, abspath);
    ASSERT_EQ(req.uri.query, "");

    abspath = "/abs/pa1h?q=val";
    http::parse_req_target(abspath, &req);
    ASSERT_EQ(req.uri.path, "/abs/pa1h");
    ASSERT_EQ(req.uri.query, "?q=val");
}

TEST(HttpTest, AuthorityForm)
{  
    http::req req;
    std::string_view path;
    
    path = "www.google.com:8080/abs/pa1h?search=waffle";
    http::parse_req_target(path, &req);
    ASSERT_EQ(req.uri.reg_name, "www.google.com");
    ASSERT_EQ(req.uri.port, 8080);
}
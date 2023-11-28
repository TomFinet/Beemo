#include <gtest/gtest.h>

#include <string_view>
#include <stdexcept>
#include <memory>

#include <http/msg.h>
#include <http/config.h>
#include <http/parser.h>
#include <http/err.h>

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
    http::parse_headers(get, req.get(), {0, {}});

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

TEST(HttpTest, ContentType)
{
    http::req req;
    req.fields["content-type"] = "text/html";    
    http::parse_content_type(&req);
    
    ASSERT_EQ(req.media_type, http::text);
    ASSERT_EQ(req.media_subtype, http::html);
}

TEST(HttpTest, ContentLength)
{
    http::req req;
    req.fields["content-length"] = "10";    
    http::parse_content_len(&req);
    
    ASSERT_EQ(req.content_len, 10);
    
    http::parse_content("howdy ho!!", &req);

    ASSERT_EQ(req.content, "howdy ho!!");

    req.fields["content-length"] = "-1";
    http::parse_content_len(&req);

    ASSERT_EQ(req.err, &http::bad_req_handler);
}

TEST(HttpTest, ContentEncodings)
{
    http::req req;
    req.fields["content-encoding"] = "gzip, compress, deflate";
    req.content_encodings = http::parse_field_list<http::encoding_t>(&req, http::content_encoding_token, &http::bad_req_handler);

    ASSERT_EQ(req.content_encodings.size(), 3);
    ASSERT_EQ(req.content_encodings[0], http::gzip);
    ASSERT_EQ(req.content_encodings[1], http::compress);
    ASSERT_EQ(req.content_encodings[2], http::deflate);

    req.content_encodings.clear();
    req.fields["content-encoding"] = "gzip,compress, deflate";
    req.content_encodings = http::parse_field_list<http::encoding_t>(&req, http::content_encoding_token, &http::bad_req_handler);

    ASSERT_EQ(req.content_encodings.size(), 3);
    ASSERT_EQ(req.content_encodings[0], http::gzip);
    ASSERT_EQ(req.content_encodings[1], http::compress);
    ASSERT_EQ(req.content_encodings[2], http::deflate);
}

TEST(HttpTest, TransferEncodings)
{
    http::req req;
    req.fields["transfer-encoding"] = "gzip,compress";
    req.transfer_encodings = http::parse_field_list<http::encoding_t>(&req, http::transfer_encoding_token, &http::not_impl_handler);

    ASSERT_EQ(req.transfer_encodings.size(), 2);
    ASSERT_EQ(req.transfer_encodings[0], http::gzip);
    ASSERT_EQ(req.transfer_encodings[1], http::compress);

    req.transfer_encodings.clear();
    req.fields["transfer-encoding"] = "gzip, compress, deflate";
    req.transfer_encodings = http::parse_field_list<http::encoding_t>(&req, http::transfer_encoding_token, &http::not_impl_handler);

    ASSERT_EQ(req.transfer_encodings.size(), 3);
    ASSERT_EQ(req.transfer_encodings[0], http::gzip);
    ASSERT_EQ(req.transfer_encodings[1], http::compress);
    ASSERT_EQ(req.transfer_encodings[2], http::deflate);
}

TEST(HttpTest, Chunked)
{
    std::string_view content =
        "c\r\n"
        "space marine\r\n"
        "a\r\n"
        " damage: 5\r\n"
        "0\r\n"
        "\r\n";

    std::string_view chunked_post =
        "POST /add HTTP/1.1\r\n"
        "Connection: keep-alive\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Content-Type: text/json\r\n"
        "Host: www.miniwargamming.com\r\n"
        "\r\n";

    http::req req;
    http::parse_headers(chunked_post, &req, {0, {}});

    ASSERT_FALSE(req.has_err());
    ASSERT_EQ(req.fields[http::transfer_encoding_token], "chunked");
    
    http::parse_content(content, &req);

    ASSERT_FALSE(req.has_err());
    ASSERT_EQ(req.transfer_encodings.size(), 1); 
    ASSERT_EQ(req.transfer_encodings[0], http::chunked); 
    ASSERT_EQ(req.content_len, 22);
    ASSERT_EQ(req.parse_state, http::complete);
    ASSERT_EQ(req.content, "space marine damage: 5");
}
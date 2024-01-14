#include <gtest/gtest.h>

#include <string_view>

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

    config config;
    req req;
    parse_headers(get, &req, {0, &config});

    ASSERT_FALSE(req.uri.has_err);
    ASSERT_TRUE(req.err == nullptr);
    ASSERT_EQ(req.method, get);
    ASSERT_EQ(req.uri.path, "/search");
    ASSERT_EQ(req.uri.query, "?q=warhammer");
    ASSERT_EQ(req.version.major, 1);
    ASSERT_EQ(req.version.minor, 1);
    ASSERT_EQ(req.fields["transfer-encoding"], "chunked");
    ASSERT_EQ(req.fields["connection"], "keep-alive");
}

TEST(HttpTest, AsterikForm)
{
    req req;

    parse_req_target("*", &req);
    ASSERT_TRUE(req.uri.asterik);

    parse_req_target("*/", &req);
    ASSERT_NE(req.err, nullptr);
    ASSERT_FALSE(req.uri.asterik);
}

TEST(HttpTest, OriginForm)
{
    req req;
    std::string_view abspath;
    
    abspath = "/abs/pa1h";
    parse_req_target(abspath, &req);
    ASSERT_EQ(req.uri.path, abspath);
    ASSERT_EQ(req.uri.query, "");

    abspath = "/abs/pa1h?q=val";
    parse_req_target(abspath, &req);
    ASSERT_EQ(req.uri.path, "/abs/pa1h");
    ASSERT_EQ(req.uri.query, "?q=val");
}

TEST(HttpTest, AuthorityForm)
{  
    req req;
    std::string_view path;
    
    path = "www.google.com:8080/abs/pa1h?search=waffle";
    parse_req_target(path, &req);
    ASSERT_EQ(req.uri.reg_name, "www.google.com");
    ASSERT_EQ(req.uri.port, 8080);
}

TEST(HttpTest, ContentType)
{
    req req;
    req.fields["content-type"] = "text/html";    
    parse_content_type(&req);
    
    ASSERT_EQ(req.media_type, text);
    ASSERT_EQ(req.media_subtype, html);
}

TEST(HttpTest, ContentLength)
{
    config config;
    req req;
    req.fields["content-length"] = "10";    
    parse_content_len(&req);
    
    ASSERT_EQ(req.content_len, 10);
    
    req.uri.port = config.transport.listening_port;
    req.uri.reg_name = config.listening_reg_name;
    req.uri.scheme = "http";
    req.fields["host"] = "google.com";
    parse_content("howdy ho!!", &req, config);

    ASSERT_EQ(req.content, "howdy ho!!");

    req.fields["content-length"] = "-1";
    parse_content_len(&req);

    ASSERT_EQ(req.err, &bad_req_handler);
}

TEST(HttpTest, ContentEncodings)
{
    req req;
    req.fields["content-encoding"] = "gzip, compress, deflate";
    req.content_encodings = parse_field_list<encoding_t>(&req, content_encoding_token, &bad_req_handler);

    ASSERT_EQ(req.content_encodings.size(), 3);
    ASSERT_EQ(req.content_encodings[0], gzip);
    ASSERT_EQ(req.content_encodings[1], compress);
    ASSERT_EQ(req.content_encodings[2], deflate);

    req.content_encodings.clear();
    req.fields["content-encoding"] = "gzip,compress, deflate";
    req.content_encodings = parse_field_list<encoding_t>(&req, content_encoding_token, &bad_req_handler);

    ASSERT_EQ(req.content_encodings.size(), 3);
    ASSERT_EQ(req.content_encodings[0], gzip);
    ASSERT_EQ(req.content_encodings[1], compress);
    ASSERT_EQ(req.content_encodings[2], deflate);
}

TEST(HttpTest, TransferEncodings)
{
    req req;
    req.fields["transfer-encoding"] = "gzip,compress";
    req.transfer_encodings = parse_field_list<encoding_t>(&req, transfer_encoding_token, &not_impl_handler);

    ASSERT_EQ(req.transfer_encodings.size(), 2);
    ASSERT_EQ(req.transfer_encodings[0], gzip);
    ASSERT_EQ(req.transfer_encodings[1], compress);

    req.transfer_encodings.clear();
    req.fields["transfer-encoding"] = "gzip, compress, deflate";
    req.transfer_encodings = parse_field_list<encoding_t>(&req, transfer_encoding_token, &not_impl_handler);

    ASSERT_EQ(req.transfer_encodings.size(), 3);
    ASSERT_EQ(req.transfer_encodings[0], gzip);
    ASSERT_EQ(req.transfer_encodings[1], compress);
    ASSERT_EQ(req.transfer_encodings[2], deflate);
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
        "Host: localhost:9001\r\n"
        "\r\n";

    config config;
    req req;
    parse_headers(chunked_post, &req, {0, &config});

    ASSERT_FALSE(req.uri.has_err);
    ASSERT_FALSE(req.has_err());
    ASSERT_EQ(req.fields[transfer_encoding_token], "chunked");
    
    parse_content(content, &req, config);

    ASSERT_FALSE(req.has_err());
    ASSERT_EQ(req.transfer_encodings.size(), 1); 
    ASSERT_EQ(req.transfer_encodings[0], chunked); 
    ASSERT_EQ(req.content_len, 22);
    ASSERT_EQ(req.parse_state, complete);
    ASSERT_EQ(req.content, "space marine damage: 5");
}

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>

#include <httparser/http_parser.h>
#include <httparser/http_req.h>
#include <httparser/http_keyword_map.h>

TEST(LexerTest, Lex_Get_Valid)
{
    std::string get = 
        "GET http://www.google.com HTTP/1.1\r"
        "Connection: keep-alive\r"
        "\r"
        "Test body...   ";

    http::http_parser parser;
    std::vector<http::line_t> headers = parser.lex(get);

    ASSERT_EQ(headers.size(), 3);
    ASSERT_EQ(headers[0].size(), 3);
    ASSERT_EQ(headers[1].size(), 2);
    ASSERT_EQ(headers[2].size(), 2);
}
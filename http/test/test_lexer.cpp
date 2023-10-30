#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>

#include <http/msg.h>
#include <http/req_parser.h>

TEST(LexerTest, Lex_Get_Valid)
{
    std::string get = 
        "GET http://www.google.com HTTP/1.1\r"
        "Connection: keep-alive\r"
        "\r"
        "Test body...   ";

    http::req req;
    http::req_parser parser(&req);
    std::vector<http::line_t> headers = parser.lex(get);

    ASSERT_EQ(headers.size(), 3);
    ASSERT_EQ(headers[0].size(), 3);
    ASSERT_EQ(headers[1].size(), 2);
    ASSERT_EQ(headers[2].size(), 2);
}
#include <gtest/gtest.h>

#include <string_view>
#include <vector>
#include <stdexcept>

#include <uri/uri.h>
#include <uri/uri_parser.h>

TEST(UriTest, Scheme)
{
    uri::uri uri;
    uri::uri_parser parser(&uri);
    std::string_view scheme;

    scheme = "dfs2dhf-+.";
    parser.parse_scheme(scheme.begin(), scheme.end());
    ASSERT_EQ(uri.scheme, scheme);

    scheme = "3dfs";
    ASSERT_THROW(parser.parse_scheme(scheme.begin(), scheme.end()), std::domain_error);

    scheme = ""; 
    ASSERT_THROW(parser.parse_scheme(scheme.begin(), scheme.end()), std::domain_error);

    scheme = "{dsf}";
    ASSERT_THROW(parser.parse_scheme(scheme.begin(), scheme.end()), std::domain_error);
}

TEST(UriTest, Host)
{
    uri::uri uri;
    uri::uri_parser parser(&uri);
    std::string_view host;

    host = "localhost";
    parser.parse_host(host.begin(), host.end());
    ASSERT_EQ(uri.reg_name, host);

    host = "[v4.fff:fff:df]";
    parser.parse_host(host.begin(), host.end());
    ASSERT_EQ(uri.future_version, "4");
    ASSERT_EQ(uri.ipvfuture, "fff:fff:df");

    host = "[vff.d3:232]";
    parser.parse_host(host.begin(), host.end());
    ASSERT_EQ(uri.ipvfuture, "d3:232");
    ASSERT_EQ(uri.future_version, "ff");

    host = "127.0.0.1";
    parser.parse_host(host.begin(), host.end());
    ASSERT_EQ(uri.ipv4, "127.0.0.1");

    host = "[vfz.d3:232]";
    ASSERT_THROW(parser.parse_host(host.begin(), host.end()), std::domain_error);
}

TEST(UriTest, Port)
{
    uri::uri uri;
    uri::uri_parser parser(&uri);
    std::string_view port;

    port = "80";
    parser.parse_port(port.begin(), port.end());
    ASSERT_EQ(uri.port, 80);

    port = "";
    parser.parse_port(port.begin(), port.end());
    ASSERT_EQ(uri.port, uri::no_port);
    
    port = "80a";
    ASSERT_THROW(parser.parse_port(port.begin(), port.end()), std::domain_error);
}

TEST(UriTest, Path)
{
    uri::uri uri;
    uri::uri_parser parser(&uri);
    std::string_view path;

    path = "";
    parser.parse_path(path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "/";
    parser.parse_path(path.begin(), path.end());
    ASSERT_EQ(uri.path, path);
    
    path = "/dsfds/gerog/dsfs"; 
    parser.parse_path(path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "first:seg/dfs";
    parser.parse_path(path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "first:seg";
    parser.parse_path(path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "//a";
    ASSERT_THROW(parser.parse_path(path.begin(), path.end()), std::domain_error);
}

TEST(UriTest, Query)
{
    uri::uri uri;
    uri::uri_parser parser(&uri);
    std::string_view query;

    query = "?x=y";
    parser.parse_query(query.begin(), query.end());
    ASSERT_EQ(uri.query, query);

    query = "?x#y";
    ASSERT_THROW(parser.parse_query(query.begin(), query.end()), std::domain_error);
}
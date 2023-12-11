#include <gtest/gtest.h>

#include <string_view>
#include <vector>
#include <stdexcept>

#include <uri/uri.h>
#include <uri/parser.h>

TEST(UriTest, Scheme)
{
    uri::uri uri;
    std::string_view scheme;

    scheme = "dfs2dhf-+.";
    uri::parse_scheme(&uri, scheme.begin(), scheme.end());
    ASSERT_EQ(uri.scheme, scheme);

    scheme = "3dfs";
    uri::parse_scheme(&uri, scheme.begin(), scheme.end());
    ASSERT_TRUE(uri.has_err);

    scheme = ""; 
    uri::parse_scheme(&uri, scheme.begin(), scheme.end());
    ASSERT_TRUE(uri.has_err);

    scheme = "{dsf}";
    uri::parse_scheme(&uri, scheme.begin(), scheme.end());
    ASSERT_TRUE(uri.has_err);
}

TEST(UriTest, Host)
{
    uri::uri uri;
    std::string_view host;

    host = "localhost";
    uri::parse_host(&uri, host.begin(), host.end());
    ASSERT_EQ(uri.reg_name, host);

    host = "[v4.fff:fff:df]";
    uri::parse_host(&uri, host.begin(), host.end());
    ASSERT_EQ(uri.future_version, "4");
    ASSERT_EQ(uri.ipvfuture, "fff:fff:df");

    host = "[vff.d3:232]";
    uri::parse_host(&uri, host.begin(), host.end());
    ASSERT_EQ(uri.ipvfuture, "d3:232");
    ASSERT_EQ(uri.future_version, "ff");

    host = "127.0.0.1";
    uri::parse_host(&uri, host.begin(), host.end());
    ASSERT_EQ(uri.ipv4, "127.0.0.1");

    host = "[vfz.d3:232]";
    uri::parse_host(&uri, host.begin(), host.end());
    ASSERT_TRUE(uri.has_err);
}

TEST(UriTest, Port)
{
    uri::uri uri;
    std::string_view port;

    port = "80";
    uri::parse_port(&uri, port.begin(), port.end());
    ASSERT_EQ(uri.port, 80);

    port = "";
    uri::parse_port(&uri, port.begin(), port.end());
    ASSERT_EQ(uri.port, uri::no_port);
    
    port = "80a";
    uri::parse_port(&uri, port.begin(), port.end());
    ASSERT_TRUE(uri.has_err);
}

TEST(UriTest, Path)
{
    uri::uri uri;
    std::string_view path;

    path = "";
    uri::parse_path(&uri, path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "/";
    uri::parse_path(&uri, path.begin(), path.end());
    ASSERT_EQ(uri.path, path);
    
    path = "/dsfds/gerog/dsfs"; 
    uri::parse_path(&uri, path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "first:seg/dfs";
    uri::parse_path(&uri, path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "first:seg";
    uri::parse_path(&uri, path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "//a";
    uri::parse_path(&uri, path.begin(), path.end());
    ASSERT_TRUE(uri.has_err);
}

TEST(UriTest, Query)
{
    uri::uri uri;
    std::string_view query;

    query = "?x=y";
    uri::parse_query(&uri, query.begin(), query.end());
    ASSERT_EQ(uri.query, query);

    query = "?x#y";
    uri::parse_query(&uri, query.begin(), query.end());
    ASSERT_TRUE(uri.has_err);
}
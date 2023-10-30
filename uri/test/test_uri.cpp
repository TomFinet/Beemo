#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <uri/uri.h>
#include <uri/uri_error.h>

TEST(UriTest, Scheme)
{
    uri::uri uri;
    std::string scheme; 

    scheme = "dfs2dhf-+.";
    uri.parse_scheme(scheme.begin(), scheme.end());
    ASSERT_EQ(uri.scheme, scheme);

    scheme = "3dfs";
    ASSERT_THROW(uri.parse_scheme(scheme.begin(), scheme.end()), uri::uri_error);

    scheme = ""; 
    ASSERT_THROW(uri.parse_scheme(scheme.begin(), scheme.end()), uri::uri_error);

    scheme = "{dsf}";
    ASSERT_THROW(uri.parse_scheme(scheme.begin(), scheme.end()), uri::uri_error);
}

TEST(UriTest, Host)
{
    uri::uri uri;
    std::string host;

    host = "localhost";
    uri.parse_host(host.begin(), host.end());
    ASSERT_EQ(uri.reg_name, host);

    host = "[v4.fff:fff:df]";
    uri.parse_host(host.begin(), host.end());
    ASSERT_EQ(uri.future_version, "4");
    ASSERT_EQ(uri.ipvfuture, "fff:fff:df");

    host = "[vff.d3:232]";
    uri.parse_host(host.begin(), host.end());
    ASSERT_EQ(uri.ipvfuture, "d3:232");
    ASSERT_EQ(uri.future_version, "ff");

    host = "127.0.0.1";
    uri.parse_host(host.begin(), host.end());
    ASSERT_EQ(uri.ipv4, "127.0.0.1");

    host = "[vfz.d3:232]";
    ASSERT_THROW(uri.parse_host(host.begin(), host.end()), uri::uri_error);
}

TEST(UriTest, Port)
{
    uri::uri uri;
    std::string port;

    port = "80";
    uri.parse_port(port.begin(), port.end());
    ASSERT_EQ(uri.port, 80);

    port = "";
    uri.parse_port(port.begin(), port.end());
    ASSERT_EQ(uri.port, uri::no_port);
    
    port = "80a";
    ASSERT_THROW(uri.parse_port(port.begin(), port.end()), uri::uri_error);
}

TEST(UriTest, Path)
{
    uri::uri uri;
    std::string path;

    path = "";
    uri.parse_path(path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "/";
    uri.parse_path(path.begin(), path.end());
    ASSERT_EQ(uri.path, path);
    
    path = "/dsfds/gerog/dsfs"; 
    uri.parse_path(path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "first:seg/dfs";
    uri.parse_path(path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "first:seg";
    uri.parse_path(path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "//a";
    ASSERT_THROW(uri.parse_path(path.begin(), path.end()), uri::uri_error);
}

TEST(UriTest, Query)
{
    uri::uri uri;
    std::string query;

    query = "?x=y";
    uri.parse_query(query.begin(), query.end());
    ASSERT_EQ(uri.query, query);

    query = "?x#y";
    ASSERT_THROW(uri.parse_query(query.begin(), query.end()), uri::uri_error);
}
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <uriparser/uri.h>
#include <uriparser/uri_error.h>

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
    ASSERT_EQ(uri.ip, "fff:fff:df");
}

TEST(UriTest, IPvFuture)
{
    uri::uri uri;
    std::string ipvfuture;

    ipvfuture = "vff.d3:232";
    uri.parse_ipvfuture(ipvfuture.begin(), ipvfuture.end());
    ASSERT_EQ(uri.ip, "d3:232");
    ASSERT_EQ(uri.future_version, "ff");

    ipvfuture = "vfz.d3:232";
    ASSERT_THROW(uri.parse_ipvfuture(ipvfuture.begin(), ipvfuture.end()), uri::uri_error);
}

TEST(UriTest, RegName)
{
    uri::uri uri;
    std::string regname;

    regname = "valid";
    uri.parse_reg_name(regname.begin(), regname.end());
    ASSERT_EQ(uri.reg_name, regname);
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

TEST(UriTest, PathAbEmpty)
{
    uri::uri uri;
    std::string path;

    path = "";
    uri.parse_path_abempty(path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "/dsfds/gerog/dsfs"; 
    uri.parse_path_abempty(path.begin(), path.end());
    ASSERT_EQ(uri.path, path);

    path = "dfds/dsf";
    ASSERT_THROW(uri.parse_path_abempty(path.begin(), path.end()), uri::uri_error);
}

TEST(UriTest, PathNoScheme)
{
    uri::uri uri;
    std::string noscheme;

    noscheme = "firstseg/dfs";
    uri.parse_path_noscheme(noscheme.begin(), noscheme.end());
    ASSERT_EQ(uri.path, noscheme);

    noscheme = "first@seg";
    uri.parse_path_noscheme(noscheme.begin(), noscheme.end());
    ASSERT_EQ(uri.path, noscheme);

    noscheme = "scheme:invalid/fail";
    ASSERT_THROW(uri.parse_path_noscheme(noscheme.begin(), noscheme.end()), uri::uri_error);
}

TEST(UriTest, PathRootless)
{
    uri::uri uri;
    std::string rootless;

    rootless = "first:seg/dfs";
    uri.parse_path_rootless(rootless.begin(), rootless.end());
    ASSERT_EQ(uri.path, rootless);

    rootless = "first:seg";
    uri.parse_path_rootless(rootless.begin(), rootless.end());
    ASSERT_EQ(uri.path, rootless);

    rootless = "/";
    ASSERT_THROW(uri.parse_path_rootless(rootless.begin(), rootless.end()), uri::uri_error);
}

TEST(UriTest, PathAbsolute)
{
    uri::uri uri;
    std::string abs;

    abs = "/";
    uri.parse_path_absolute(abs.begin(), abs.end());
    ASSERT_EQ(uri.path, abs);

    abs = "/d/a";
    uri.parse_path_absolute(abs.begin(), abs.end());
    ASSERT_EQ(uri.path, abs);

    abs = "//a";
    ASSERT_THROW(uri.parse_path_absolute(abs.begin(), abs.end()), uri::uri_error);

    abs = "";
    ASSERT_THROW(uri.parse_path_absolute(abs.begin(), abs.end()), uri::uri_error);
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
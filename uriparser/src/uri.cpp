#include <uriparser/uri.h>
#include <uriparser/uri_error.h>

#include <iostream>

namespace uri {

void uri::parse_uri(const std::string &uri_str, int flags)
{
    size_t auth_start_pos = uri_str.find("//");
    size_t query_start_pos = uri_str.find('?');
    size_t path_start_pos;

    if (flags & flag_scheme) {
        size_t scheme_end = uri_str.find("://"); /* assumes scheme always comes with authority, is this true? */
        if (scheme_end != std::string::npos) {
            parse_scheme(uri_str.begin(), uri_str.begin() + scheme_end);
        }
    }

    if (auth_start_pos == std::string::npos) {
        auth_start_pos = 0;
        path_start_pos = uri_str.find('/');
    }
    else {
        path_start_pos = uri_str.find('/', auth_start_pos + 2);
    } 

    if (flags & flag_authority) {
        /* authority has follow set '/', '?' or end of string. */
        if (path_start_pos != std::string::npos) {
            parse_authority(uri_str.begin() + auth_start_pos, uri_str.begin() + path_start_pos);
        }
        else if (query_start_pos != std::string::npos) {
            parse_authority(uri_str.begin() + auth_start_pos, uri_str.begin() + query_start_pos);
        }
        else {
            parse_authority(uri_str.begin() + auth_start_pos, uri_str.end());
        }
    }

    if (flags & flag_path && path_start_pos != std::string::npos) {
        if (query_start_pos == std::string::npos) {
            parse_path(uri_str.begin() + path_start_pos, uri_str.end());
        }
        else {
            parse_path(uri_str.begin() + path_start_pos, uri_str.begin() + query_start_pos);
        }
    }

    if (flags & flag_query && query_start_pos != std::string::npos) {
        parse_query(uri_str.begin() + query_start_pos, uri_str.end());
    }
}

/* alpha *(alpha | digit | + | - | . ) */
void uri::parse_scheme(std::string::const_iterator scheme_start,
                       std::string::const_iterator scheme_end) 
{
    if (scheme_start == scheme_end) {
        throw uri_error("Scheme cannot be empty.");
    }

    if (!std::isalpha(*scheme_start)) {
        throw uri_error("Scheme must begin with an alphabetic character.");
    }

    scheme = parse_pattern(scheme_start, scheme_end, [](char c)
    {
        return c == '+' || c == '-' || c == '.' || std::isdigit(c) || std::isalpha(c);
    });
}

void uri::parse_authority(std::string::const_iterator auth_start,
                          std::string::const_iterator auth_end)
{
    std::string auth_str(auth_start, auth_end);

    size_t userinfo_end = auth_str.find('@');
    size_t host_start;
    if (userinfo_end != std::string::npos) {
        parse_userinfo(auth_str.begin(), auth_str.begin() + userinfo_end);
        host_start = userinfo_end + 1;
    }
    else {
        host_start = 0;
    }

    size_t ip_literal_end = auth_str.find(']');
    size_t port_start;
    if (ip_literal_end == std::string::npos) {
        port_start = auth_str.find(':');
    }
    else {
        port_start = auth_str.find(':', ip_literal_end);
    }

    if (port_start == std::string::npos) {
        parse_host(auth_str.begin() + host_start, auth_str.end());
    }
    else {
        parse_host(auth_str.begin() + host_start, auth_str.begin() + port_start);
        parse_port(auth_str.begin() + port_start + 1, auth_str.end());
    }
}

void uri::parse_userinfo(std::string::const_iterator userinfo_start,
                         std::string::const_iterator userinfo_end)
{
    userinfo = parse_pattern(userinfo_start, userinfo_end, [](char c)
    {
        return is_unreserved(c) || is_sub_delimiter(c) || ':';
    });
}

void uri::parse_host(std::string::const_iterator host_start,
                     std::string::const_iterator host_end)
{
    if (host_start == host_end) {
        throw uri_error("Host cannot be empty.");
    }

    if (*host_start == '[') {
        if (*(host_end - 1) != ']') {
            throw uri_error("IP literal must end with ']'.");
        }

        if (std::tolower(*(host_start + 1)) == 'v') {
            parse_ipvfuture(host_start + 1, host_end - 1);
        }
        else {
            std::string host(host_start + 1, host_end - 1); 
            ipv6 = host;
        }
    }
    else {
        std::string host = parse_pattern(host_start, host_end, [](char c)
        {
            return is_unreserved(c) || is_sub_delimiter(c);
        });

        ipv4 = host;
        reg_name = host;
    }
}

/* "v" 1*HEXDIG "." 1*( unreserved | sub-delims | ":" ) */
void uri::parse_ipvfuture(std::string::const_iterator ipvfuture_start,
                          std::string::const_iterator ipvfuture_end)
{
    if (ipvfuture_start == ipvfuture_end) {
        throw uri_error("IPvFuture cannot be empty.");
    }

    if (*ipvfuture_start != 'v') {
        throw uri_error("IPvFuture must start with 'v'.");
    }

    std::string ipvfuture_str(ipvfuture_start, ipvfuture_end); 
    size_t dot_pos = ipvfuture_str.find('.');
    if (dot_pos == std::string::npos) {
        throw uri_error("IPvFuture must delimite its version number by a '.' .");
    }

    auto version_end = ipvfuture_start + dot_pos;
    std::string version = parse_pattern(ipvfuture_start + 1, version_end, std::isxdigit);

    if (version.empty()) {
        throw uri_error("IPvFuture version number cannot be empty.");
    }

    future_version = version;

    ipvfuture = parse_pattern(version_end + 1, ipvfuture_end, [](char c)
    {
        return is_unreserved(c) || is_sub_delimiter(c) ||  c == ':';            
    });

    if (ipvfuture.empty()) {
        throw uri_error("IPvFuture must have an ip address.");
    }
}

void uri::parse_port(std::string::const_iterator port_start,
                     std::string::const_iterator port_end) 
{
    std::string port_str = parse_pattern(port_start, port_end, std::isdigit);
    if (port_str.empty()) {
        port = no_port;
    }
    else {
        port = std::stoi(port_str);
    }
}

void uri::parse_path(std::string::const_iterator path_start,
                     std::string::const_iterator path_end)
{
    path.clear();

    auto it = path_start;
    while (it != path_end && is_pchar(*it)) { 
        path.push_back(*it);
        it++;
    }

    while (it != path_end) {
        if (*it != '/') {
            throw uri_error("Path must start with a '/'.");
        }

        path.push_back('/');
        it++;

        bool seg_exists = false;
        while (it != path_end && *it != '/') {
            if (!is_pchar(*it)) {
                throw uri_error("Path segment has invalid character.");
            }

            seg_exists = true;
            path.push_back(*it);
            it++;
        }

        if (!seg_exists && it != path_end) {
            throw uri_error("Path segment is missing.");
        }
    }
}

void uri::parse_query(std::string::const_iterator query_start,
                      std::string::const_iterator query_end)
{
    query = parse_pattern(query_start, query_end, [](char c)
    {
        return is_pchar(c) || c == '/' || c == '?';
    });
}

template<typename P>
std::string uri::parse_pattern(std::string::const_iterator pattern_start,
                               std::string::const_iterator pattern_end,
                               P&& pattern)
{
    std::string pattern_str;

    for (auto iter = pattern_start; iter != pattern_end; iter++) {
        if (!pattern(*iter)) {
            throw uri_error("Unexpected character.");
        }

        pattern_str.push_back(*iter);
    }

    return pattern_str;
}

}
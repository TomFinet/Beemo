#include <uriparser/uri.h>
#include <uriparser/uri_error.h>

#include <iostream>

namespace uri {

/* alpha *(alpha | digit | + | - | . ) */
void uri::parse_scheme(std::string::const_iterator scheme_start,
                       std::string::const_iterator scheme_end) 
{
    if (scheme_start == scheme_end) {
        throw uri_error("Scheme cannot be empty.");
    }

    std::string scheme = parse_pattern(scheme_start, scheme_end, [](char c)
    {
        return c == '+' || c == '-' || c == '.' || std::isdigit(c) || std::isalpha(c); 
    });

    if (!std::isalpha(scheme.at(0))) {
        throw uri_error("Scheme must start with a alphabetic character.");
    }
    
    this->scheme = scheme;
}

// TODO: not sure selection between host types is correct.
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
            parse_ipv6(host_start + 1, host_end - 1); 
        }
    }
    else if (is_unreserved(*host_start) || is_sub_delimiter(*host_start)) {
        parse_reg_name(host_start, host_end);    
    }
    else if (std::isdigit(*host_start)) {
        parse_ipv4(host_start, host_end);
    }
    else {
        throw uri_error("Unrecognised host.");
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

    std::string ipvfuture = parse_pattern(version_end + 1, ipvfuture_end, [](char c)
    {
        return is_unreserved(c) || is_sub_delimiter(c) ||  c == ':';            
    });

    if (ipvfuture.empty()) {
        throw uri_error("IPvFuture must have an ip address.");
    }

    ip = ipvfuture; 
}

void uri::parse_ipv6(std::string::const_iterator ipv6_start,
                     std::string::const_iterator ipv6_end)
{
    // TODO: check format is correct for now we assume it is.
    std::string ipv6(ipv6_start, ipv6_end);
    ip = ipv6;
}

void uri::parse_ipv4(std::string::const_iterator ipv4_start,
                     std::string::const_iterator ipv4_end)
{
    std::string ipv4(ipv4_start, ipv4_end);
    ip = ipv4;
}

/* *( unreserved | pct-encoded | sub-delims ) */
void uri::parse_reg_name(std::string::const_iterator reg_name_start,
                         std::string::const_iterator reg_name_end)
{
    reg_name = parse_pattern(reg_name_start, reg_name_end, [](char c)
    {
        // TODO: does not allow for percentage encoding.
        return is_unreserved(c) || is_sub_delimiter(c);
    });
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

void uri::parse_absolute_uri(const std::string &uri_str)
{
    size_t scheme_end_pos = uri_str.find(':');
    size_t query_start_pos = uri_str.find('?');
    
    if (scheme_end_pos == std::string::npos) {
        throw uri_error("Absolute URI scheme must be delimited by a ':'.");
    }

    if (query_start_pos == std::string::npos) {
        query_start_pos = uri_str.length();
    }

    parse_scheme(uri_str.begin(), uri_str.begin() + scheme_end_pos);
    //parse_hierarchy_part(uri_str.begin() + scheme_end_pos + 1, uri_str.begin() + query_start_pos);
    parse_query(uri_str.begin() + query_start_pos, uri_str.end());
}

void uri::parse_path_abempty(std::string::const_iterator path_start,
                             std::string::const_iterator path_end)
{
    parse_path_helper(path_start, path_end);    
}

void uri::parse_path_noscheme(std::string::const_iterator path_start,
                              std::string::const_iterator path_end)
{
    parse_segment_path_helper(path_start, path_end, [](char c)
    {
        return is_pchar(c) && c != ':';
    });
}

void uri::parse_path_rootless(std::string::const_iterator path_start,
                              std::string::const_iterator path_end)
{
    parse_segment_path_helper(path_start, path_end, is_pchar);
}

void uri::parse_path_absolute(std::string::const_iterator path_start,
                              std::string::const_iterator path_end)
{
    if (path_start == path_end) {
        throw uri_error("Absolute path cannot be empty.");
    }

    parse_path_helper(path_start, path_end);
}

void uri::parse_query(std::string::const_iterator query_start,
                      std::string::const_iterator query_end)
{
    query = parse_pattern(query_start, query_end, [](char c)
    {
        return is_pchar(c) || c == '/' || c == '?';
    });
}

void uri::parse_path_helper(std::string::const_iterator path_start,
                            std::string::const_iterator path_end)
{
    std::string p;

    for (auto iter = path_start; iter != path_end;) {
        if (*iter != '/') {
            throw uri_error("Path must start with a '/'.");
        }

        p.push_back('/');
        iter++;

        bool seg_exists = false;
        while (iter != path_end && *iter != '/') {
            if (!is_pchar(*iter)) {
                throw uri_error("Path segment has invalid character.");
            }

            seg_exists = true;
            p.push_back(*iter);
            iter++;
        }

        if (!seg_exists && iter != path_end) {
            throw uri_error("Path segment is missing.");
        }
    }

    path = p;
}

template<typename P>
void uri::parse_segment_path_helper(std::string::const_iterator path_start,
                                    std::string::const_iterator path_end,
                                    P&& pattern)
{
    std::string first_segment;

    if (path_start == path_end) {
        throw uri_error("Path cannot be empty.");
    } 

    if (*path_start == '/') {
        throw uri_error("Path cannot start with '/'.");
    }

    auto iter = path_start;
    while (iter != path_end && *iter != '/') {
        if (!(pattern(*iter))) {
            throw uri_error("Unrecognised character in first segment of path.");
        }
        first_segment.push_back(*iter);
        iter++;
    }

    parse_path_helper(iter, path_end);
    path = first_segment + path;
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
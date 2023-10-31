#include <uri/uri_parser.h>

#include <string>
#include <stdexcept>

namespace uri {

    void uri_parser::parse_uri(std::string_view uri_str, int flags)
    {
        size_t auth_start_pos = uri_str.find("//");
        const size_t query_start_pos = uri_str.find('?');
        size_t path_start_pos;

        if (flags & flag_scheme) {
            const size_t scheme_end = uri_str.find("://"); /* assumes scheme always comes with authority, is this true? */
            if (scheme_end != std::string::npos) {
                parse_scheme(uri_str.begin(), uri_str.begin() + scheme_end);
            }
            else {
                uri->scheme = no_scheme;
            }
        }

        if (auth_start_pos == std::string_view::npos) {
            auth_start_pos = 0;
            path_start_pos = uri_str.find('/');
        }
        else {
            path_start_pos = uri_str.find('/', auth_start_pos + 2);
        } 

        if (flags & flag_authority) {
            /* authority has follow set '/', '?' or end of string. */
            if (path_start_pos != std::string_view::npos) {
                parse_authority(uri_str.begin() + auth_start_pos, uri_str.begin() + path_start_pos);
            }
            else if (query_start_pos != std::string_view::npos) {
                parse_authority(uri_str.begin() + auth_start_pos, uri_str.begin() + query_start_pos);
            }
            else {
                parse_authority(uri_str.begin() + auth_start_pos, uri_str.end());
            }
        }

        if (flags & flag_path && path_start_pos != std::string_view::npos) {
            if (query_start_pos == std::string_view::npos) {
                parse_path(uri_str.begin() + path_start_pos, uri_str.end());
            }
            else {
                parse_path(uri_str.begin() + path_start_pos, uri_str.begin() + query_start_pos);
            }
        }

        if (flags & flag_query && query_start_pos != std::string_view::npos) {
            parse_query(uri_str.begin() + query_start_pos, uri_str.end());
        }
    }

    /* alpha *(alpha | digit | + | - | . ) */
    void uri_parser::parse_scheme(std::string_view::const_iterator scheme_start,
                                  std::string_view::const_iterator scheme_end) 
    {
        if (scheme_start == scheme_end) {
            throw std::domain_error("Scheme cannot be empty.");
        }

        if (!std::isalpha(*scheme_start)) {
            throw std::domain_error("Scheme must begin with an alphabetic character.");
        }

        parse_pattern(scheme_start, scheme_end, [](char c)
        {
            return c == '+' || c == '-' || c == '.' || std::isdigit(c) || std::isalpha(c);
        });
        std::string scheme(scheme_start, scheme_end);
        uri->scheme = scheme;
    }

    void uri_parser::parse_authority(std::string_view::const_iterator auth_start,
                                     std::string_view::const_iterator auth_end)
    {
        std::string_view auth_str(auth_start, auth_end);

        const size_t userinfo_end = auth_str.find('@');
        size_t host_start;
        if (userinfo_end != std::string::npos) {
            parse_userinfo(auth_str.begin(), auth_str.begin() + userinfo_end);
            host_start = userinfo_end + 1;
        }
        else {
            host_start = 0;
        }

        const size_t ip_literal_end = auth_str.find(']');
        size_t port_start;
        if (ip_literal_end == std::string_view::npos) {
            port_start = auth_str.find(':');
        }
        else {
            port_start = auth_str.find(':', ip_literal_end);
        }

        if (port_start == std::string_view::npos) {
            parse_host(auth_str.begin() + host_start, auth_str.end());
            uri->port = no_port;
        }
        else {
            parse_host(auth_str.begin() + host_start, auth_str.begin() + port_start);
            parse_port(auth_str.begin() + port_start + 1, auth_str.end());
        }
    }

    void uri_parser::parse_userinfo(std::string_view::const_iterator userinfo_start,
                                    std::string_view::const_iterator userinfo_end)
    {
        parse_pattern(userinfo_start, userinfo_end, [](char c)
        {
            return is_unreserved(c) || is_sub_delimiter(c) || ':';
        });
        std::string userinfo(userinfo_start, userinfo_end);
        uri->userinfo = userinfo;
    }

    void uri_parser::parse_host(std::string_view::const_iterator host_start,
                                std::string_view::const_iterator host_end)
    {
        if (host_start == host_end) {
            throw std::domain_error("Host cannot be empty.");
        }

        if (*host_start == '[') {
            if (*(host_end - 1) != ']') {
                throw std::domain_error("IP literal must end with ']'.");
            }

            if (std::tolower(*(host_start + 1)) == 'v') {
                parse_ipvfuture(host_start + 1, host_end - 1);
            }
            else {
                std::string host(host_start + 1, host_end - 1); 
                uri->ipv6 = host;
            }
        }
        else {
            parse_pattern(host_start, host_end, [](char c)
            {
                return is_unreserved(c) || is_sub_delimiter(c);
            });
            std::string host(host_start, host_end);
            uri->ipv4 = host;
            uri->reg_name = host;
        }
    }

    /* "v" 1*HEXDIG "." 1*( unreserved | sub-delims | ":" ) */
    void uri_parser::parse_ipvfuture(std::string_view::const_iterator ipvfuture_start,
                                     std::string_view::const_iterator ipvfuture_end)
    {
        if (ipvfuture_start == ipvfuture_end) {
            throw std::domain_error("IPvFuture cannot be empty.");
        }

        if (*ipvfuture_start != 'v') {
            throw std::domain_error("IPvFuture must start with 'v'.");
        }

        std::string ipvfuture_str(ipvfuture_start, ipvfuture_end); 
        size_t dot_pos = ipvfuture_str.find('.');
        if (dot_pos == std::string::npos) {
            throw std::domain_error("IPvFuture must delimite its version number by a '.' .");
        }

        auto version_end = ipvfuture_start + dot_pos;
        parse_pattern(ipvfuture_start + 1, version_end, std::isxdigit);
        std::string version(ipvfuture_start + 1, version_end);

        if (version.empty()) {
            throw std::domain_error("IPvFuture version number cannot be empty.");
        }

        uri->future_version = version;

        parse_pattern(version_end + 1, ipvfuture_end, [](char c)
        {
            return is_unreserved(c) || is_sub_delimiter(c) ||  c == ':';            
        });
        std::string ipvfuture(version_end + 1, ipvfuture_end);

        if (ipvfuture.empty()) {
            throw std::domain_error("IPvFuture must have an ip address.");
        }
        
        uri->ipvfuture = ipvfuture;
    }

    void uri_parser::parse_port(std::string_view::const_iterator port_start,
                                std::string_view::const_iterator port_end) 
    {
        parse_pattern(port_start, port_end, std::isdigit);
        std::string port(port_start, port_end);
        if (port.empty()) {
            uri->port = no_port;
        }
        else {
            uri->port = std::stoi(port);
        }
    }

    void uri_parser::parse_path(std::string_view::const_iterator path_start,
                                std::string_view::const_iterator path_end)
    {
        auto it = path_start;
        while (it != path_end && is_pchar(*it)) { 
            it++;
        }

        while (it != path_end) {
            if (*it != '/') {
                throw std::domain_error("Path must start with a '/'.");
            }

            it++;

            bool seg_exists = false;
            while (it != path_end && *it != '/') {
                if (!is_pchar(*it)) {
                    throw std::domain_error("Path segment has invalid character.");
                }

                seg_exists = true;
                it++;
            }

            if (!seg_exists && it != path_end) {
                throw std::domain_error("Path segment is missing.");
            }
        }

        std::string path(path_start, path_end);
        uri->path = path;
    }

    void uri_parser::parse_query(std::string_view::const_iterator query_start,
                                 std::string_view::const_iterator query_end)
    {
        parse_pattern(query_start, query_end, [](char c)
        {
            return is_pchar(c) || c == '/' || c == '?';
        });
        std::string query(query_start, query_end);
        uri->query = query;
    }

    template<typename P>
    void uri_parser::parse_pattern(std::string_view::const_iterator pattern_start,
                                   std::string_view::const_iterator pattern_end,
                                   P&& pattern)
    {
        for (auto iter = pattern_start; iter != pattern_end; iter++) {
            if (!pattern(*iter)) {
                throw std::domain_error("Unexpected character.");
            }
        }
    }

}
#pragma once

#include <string>

namespace uri {

const int no_port = -1;
const std::string no_scheme = "";

const int flag_scheme = 1;
const int flag_authority = 2;
const int flag_path = 4;
const int flag_query = 8;
const int flag_all = flag_scheme | flag_authority | flag_path | flag_query;

class uri {

    public:
        
        bool asterik;
        std::string scheme;
        std::string userinfo;
        std::string future_version;
        std::string ipv4;
        std::string ipv6;
        std::string ipvfuture;
        std::string reg_name;
        int port;
        std::string path;
        std::string query;

        uri() { }
        ~uri() { }

        void parse_uri(const std::string &uri_str, int flags);


        void parse_scheme(std::string::const_iterator scheme_start,
                          std::string::const_iterator scheme_end);

        void parse_authority(std::string::const_iterator auth_start,
                             std::string::const_iterator auth_end);

        void parse_userinfo(std::string::const_iterator userinfo_start,
                            std::string::const_iterator userinfo_end);

        void parse_host(std::string::const_iterator host_start,
                        std::string::const_iterator host_end);

        void parse_ipvfuture(std::string::const_iterator ipvfuture_start,
                             std::string::const_iterator ipvfuture_end);

        void parse_port(std::string::const_iterator port_start,
                        std::string::const_iterator port_end); 

        void parse_path(std::string::const_iterator path_start,
                        std::string::const_iterator path_end);
        
        void parse_query(std::string::const_iterator query_start,
                         std::string::const_iterator query_end);

        template<typename P>
        std::string parse_pattern(std::string::const_iterator pattern_start,
                                  std::string::const_iterator pattern_end,
                                  P&& pattern);

    private:

        static inline bool is_unreserved(char c)
        {
            return std::isalpha(c) || std::isdigit(c) || c == '-' ||
                   c == '.' || c == '_' || c == '~';
        }

        static inline bool is_reserved(char c)
        {
            return is_gen_delimiter(c) || is_sub_delimiter(c);
        }

        static inline bool is_gen_delimiter(char c)
        {
            return c == ':' || c == '/' || c == '?' || c == '#' ||
                   c == '[' || c == ']' || c == '@';
        }
         
        static inline bool is_sub_delimiter(char c)
        {
            return c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' ||
                   c == '*' || c == '+' || c == ',' || c == ';' || c == '=';
        }

        static inline bool is_pchar(char c)
        {
            return is_unreserved(c) || is_sub_delimiter(c) || c == ':' || c == '@';
        }
};

}
#pragma once

#include <string>

namespace uri {

const int no_port = -1;

class uri {

    public:
        
        bool asterik;
        std::string scheme;
        std::string authority;
        std::string future_version;
        std::string ip;
        std::string reg_name;
        int port;
        std::string path;
        std::string query;
        std::string fragment;

        uri() { }
        ~uri() { }

        void parse_scheme(std::string::const_iterator scheme_start,
                          std::string::const_iterator scheme_end);

        void parse_host(std::string::const_iterator host_start,
                        std::string::const_iterator host_end);

        void parse_ipv6(std::string::const_iterator ipv6_start,
                        std::string::const_iterator ipv6_end);

        void parse_ipvfuture(std::string::const_iterator ipvfuture_start,
                             std::string::const_iterator ipvfuture_end);

        void parse_ipv4(std::string::const_iterator ipv4_start,
                        std::string::const_iterator ipv4_end);

        void parse_reg_name(std::string::const_iterator reg_name_start,
                            std::string::const_iterator reg_name_end);

        void parse_port(std::string::const_iterator port_start,
                        std::string::const_iterator port_end); 

        void parse_absolute_uri(const std::string &abs_uri);

        void parse_path_abempty(std::string::const_iterator path_start,
                                std::string::const_iterator path_end);

        void parse_path_absolute(std::string::const_iterator path_start,
                                 std::string::const_iterator path_end);

        void parse_path_noscheme(std::string::const_iterator path_start,
                                 std::string::const_iterator path_end);
        
        void parse_path_rootless(std::string::const_iterator path_start,
                                 std::string::const_iterator path_end);

        void parse_path_helper(std::string::const_iterator path_start,
                               std::string::const_iterator path_end);
        
        template<typename P>
        void parse_segment_path_helper(std::string::const_iterator path_start,
                                       std::string::const_iterator path_end,
                                       P&& pattern);

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
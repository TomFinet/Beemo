#include <uri/uri.h>

#include <string_view>


namespace uri {

    constexpr int no_port = -1;
    const std::string no_scheme = "";

    constexpr int flag_scheme = 1;
    constexpr int flag_authority = 2;
    constexpr int flag_path = 4;
    constexpr int flag_query = 8;
    constexpr int flag_all = flag_scheme | flag_authority | flag_path | flag_query;


    class uri_parser {

        public:
            
            struct uri *uri;

            uri_parser(struct uri *const uri) : uri(uri) { }

            ~uri_parser() = default;

            void parse_uri(std::string_view uri_str, int flags);

            void parse_scheme(std::string_view::const_iterator scheme_start,
                            std::string_view::const_iterator scheme_end);

            void parse_authority(std::string_view::const_iterator auth_start,
                                std::string_view::const_iterator auth_end);

            void parse_userinfo(std::string_view::const_iterator userinfo_start,
                                std::string_view::const_iterator userinfo_end);

            void parse_host(std::string_view::const_iterator host_start,
                            std::string_view::const_iterator host_end);

            void parse_ipvfuture(std::string_view::const_iterator ipvfuture_start,
                                std::string_view::const_iterator ipvfuture_end);

            void parse_port(std::string_view::const_iterator port_start,
                            std::string_view::const_iterator port_end); 

            void parse_path(std::string_view::const_iterator path_start,
                            std::string_view::const_iterator path_end);
            
            void parse_query(std::string_view::const_iterator query_start,
                            std::string_view::const_iterator query_end);

            template<typename P>
            void parse_pattern(std::string_view::const_iterator pattern_start,
                            std::string_view::const_iterator pattern_end,
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
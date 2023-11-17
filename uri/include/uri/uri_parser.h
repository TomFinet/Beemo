#include <uri/uri.h>

#include <string_view>


namespace uri
{

    constexpr int no_port = -1;
    constexpr auto &no_scheme = "";
    
    constexpr int flag_scheme = 1;
    constexpr int flag_authority = 2;
    constexpr int flag_path = 4;
    constexpr int flag_query = 8;
    constexpr int flag_all = flag_scheme | flag_authority | flag_path | flag_query;


    void parse_uri(struct uri *const uri, std::string_view raw_uri, int flags);

    void parse_scheme(struct uri *const uri,
                      std::string_view::const_iterator scheme_start,
                      std::string_view::const_iterator scheme_end);

    void parse_authority(struct uri *const uri,
                         std::string_view::const_iterator auth_start,
                         std::string_view::const_iterator auth_end);

    void parse_userinfo(struct uri *const uri,
                        std::string_view::const_iterator userinfo_start,
                        std::string_view::const_iterator userinfo_end);

    void parse_host(struct uri *const uri,
                    std::string_view::const_iterator host_start,
                    std::string_view::const_iterator host_end);

    void parse_ipvfuture(struct uri *const uri,
                         std::string_view::const_iterator ipvfuture_start,
                         std::string_view::const_iterator ipvfuture_end);

    void parse_port(struct uri *const uri,
                    std::string_view::const_iterator port_start,
                    std::string_view::const_iterator port_end); 

    void parse_path(struct uri *const uri,
                    std::string_view::const_iterator path_start,
                    std::string_view::const_iterator path_end);
    
    void parse_query(struct uri *const uri,
                     std::string_view::const_iterator query_start,
                     std::string_view::const_iterator query_end);

}
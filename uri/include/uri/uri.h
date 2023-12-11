#pragma once

#include <string>


namespace uri {

    struct uri {
        bool has_err;
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
    };
}

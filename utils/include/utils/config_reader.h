#pragma once

#include <string_view>

/* TODO: we want some means of configuring our server from a text file, parsing it and using it at runtime. */
/* NOTE: this can all be done at compile time. constexpr is looking handy here. */

namespace utils
{
    namespace config
    {
        
        void read_config(std::string_view config_path)
        {
            
        }

    }
}
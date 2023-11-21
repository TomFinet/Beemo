#pragma once

#include <string_view>
#include <string>
#include <vector>
#include <stdexcept>

namespace utils
{

    template<typename P>
    bool parse_pattern(std::string_view::const_iterator pattern_start,
                       std::string_view::const_iterator pattern_end,
                       P&& pattern)
    {
        for (auto iter = pattern_start; iter != pattern_end; iter++) {
            if (!pattern(*iter)) {
                return false;
            }
        }
        return true;
    }

    /* parses strings of the form: "s1, s2, s3" into a container, preserving the order. */
    void parse_list(std::string_view list, std::vector<std::string> *parsed);

    std::string lowercase_str(std::string_view s);
}
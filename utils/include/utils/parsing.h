#pragma once

#include <string_view>
#include <stdexcept>

namespace utils
{

    template<typename P>
    void parse_pattern(std::string_view::const_iterator pattern_start,
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
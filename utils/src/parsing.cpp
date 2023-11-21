#include <utils/parsing.h>

#include <ranges>
#include <algorithm>

namespace utils 
{

    std::string lowercase_str(std::string_view s)
    {
        auto lowered = std::views::transform(s, std::tolower); 
        return std::string(lowered.begin(), lowered.end());
    }

    void parse_list(std::string_view list, std::vector<std::string> *parsed)
    {
        for (std::string_view::const_iterator it = list.begin(); it != list.end();) {

            std::string_view::const_iterator start = it;
            while (it != list.end() && *it != ',') {
                it++;
            }
            std::string_view::const_iterator end = it;

            if (start == end) {
                goto err;
            }

            parsed->emplace_back(std::string(start, end));

            if (end != list.end()) {
                it = end + 1;
                if (end + 1 != list.end() && *(end + 1) == ' ') {
                    it++;
                }
            }
        }
        return;
    err:
        parsed = nullptr;
    }

}
#include <http/msg.h>


namespace beemo
{
    bool req::has_err(void) const
    {
        return err != nullptr;
    }

    bool req::is_parsing_headers(void) const
    {
        return (parse_state == start_line || parse_state == headers) && !has_err();
    }

    bool req::is_parsing_incomplete(void) const
    {
        return parse_state != complete && !has_err();
    }

    void resp::put_header(const std::string &header, const std::string &value)
    {
        fields[header] = value;
    }
    
    std::string resp::to_str(void)
    {
        std::stringstream ss;
        ss << "HTTP/" << version.major << "." << version.minor << " " << status_code << " " << reason << "\r\n";

        for (auto &field : fields) {
            ss << field.first << ": " << field.second << "\r\n";
        }

        if (!content.empty()) {
            ss << "\r\n" << content;
        }

        return ss.str();
    }
}
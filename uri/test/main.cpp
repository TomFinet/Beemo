#include <string_view>
#include <iostream>
#include <stdexcept>

#include <uri/uri.h>
#include <uri/parser.h>

int main()
{
    uri::uri uri;
    uri::uri_parser parser(&uri);
    std::string_view host, path;

    path = "/d/";
    try {
        parser.parse_path(path.begin(), path.end());
    }
    catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }
}
#include <http/msg.h>

#include <iostream>


namespace http {

    void req::print(void) const {
        std::cout << "method: " << method << std::endl;
        std::cout << "version: " << version.major << "." << version.minor << std::endl;
        std::cout << "reg_name: " << uri.reg_name << std::endl; 
        std::cout << "port: " << uri.port << std::endl;
        std::cout << "scheme: " << uri.scheme << std::endl;
        std::cout << "path: " << uri.path << std::endl;

        for (auto &x : fields) {
            std::cout << x.first << ": " << x.second << std::endl;
        }

        std::cout << std::endl;
    }

    bool req::has_err(void) const
    {
        return err != nullptr;
    }

}
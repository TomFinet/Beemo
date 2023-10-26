#pragma once

#include <stdexcept> 
#include <string>

namespace http {

class http_error : public std::domain_error {

    public:
        http_error(const std::string &err_msg) throw() : std::domain_error(err_msg) { }

        const char* what() const throw()
        {
            return std::domain_error::what();
        }
};

}
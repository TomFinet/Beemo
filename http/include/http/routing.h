#pragma once

#include <http/msg.h>

#include <string>
#include <functional>


namespace http
{

    struct req_id {
        std::string path;
        method_t method;

        bool operator==(const req_id &id) const
        {
            return path == id.path && method == id.method;
        } 
    };

    struct req_hasher {
        size_t operator()(const req_id &id) const
        {
            return std::hash<std::string>()(id.path) ^ std::hash<method_t>()(id.method);
        }
    };

    typedef std::function<void(struct req *)> req_handler_t;


    /* the first step to routing is to ensure the message is semantically correct. */ 
    void validate(req *const req, const struct config &config);

    void route_to_resource_handler(req *const req);

    void register_req_handler(const req_id &req_id, req_handler_t req_handler);

}
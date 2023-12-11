#pragma once

#include <http/msg.h>

#include <string>
#include <functional>
#include <memory>


namespace http
{

    /* TODO: this hash function sucks, and will incurr more collisions than others. */
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

    /* TODO: modernise this. */
    typedef std::string (*req_handler_t)(struct req *);

    std::unique_ptr<struct response> route_to_resource_handler(req *const req);

    void register_req_handler(const req_id &req_id, req_handler_t req_handler);

}
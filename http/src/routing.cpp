#include <http/routing.h>
#include <http/config.h>
#include <http/err.h>

#include <unordered_map>
#include <iostream>


namespace beemo
{
    using req_handler_map_t = std::unordered_map<struct req_id, req_handler_t, struct req_hasher>; 
    req_handler_map_t req_to_handler;

    std::unique_ptr<struct resp> route_to_resource_handler(req *const req)
    {
        req_id req_id {req->uri.path, req->method};
        std::unique_ptr<struct resp> response = std::make_unique<struct resp>();

        if (!req_to_handler.contains(req_id)) {
            goto err_not_found;
        }

        try {
            response->content = req_to_handler[req_id](req);
        }
        catch (std::exception) {
            goto err_internal;
        }

        switch (req->method) {
            case get:
                response->status_code = 200;
                response->reason = "OK";
                break;
            case post:
                response->status_code = 201;
                response->reason = "Created";
                break;
        }
        response->put_header("Content-Length", std::to_string(response->content.size()));
        return std::move(response);

    err_not_found:
        req->err = &not_found_handler;
        return std::make_unique<struct resp>();
    err_internal:
        req->err = &internal_server_handler;
        return std::make_unique<struct resp>();
    }

    void register_req_handler(const req_id &req_id, req_handler_t req_handler)
    {
        req_to_handler[req_id] = req_handler;
    }
}
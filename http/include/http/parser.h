#pragma once

#include <string_view>
#include <string>

#include <http/msg.h>
#include <http/config.h>
#include <http/err.h>
#include <http/keyword_map.h>

#include <utils/parsing.h>


namespace beemo {

    struct parse_ctx {
        const size_t start_idx;
        const struct config *const config;
    };
    
    size_t parse_headers(std::string_view raw_req, struct req *const req, const struct parse_ctx &ctx);
    void parse_req_line(std::string_view req_line, struct req *const req, const size_t max_req_line_len);
    void parse_req_target(std::string_view req_target, req *const req);
    void parse_version(std::string_view version, req *const req);
    void parse_field_line(std::string_view field_line, req *const req);
    void parse_host(std::string_view host_val, req *const req);
    
    void parse_content_len(struct req *const req);
    void parse_content_type(req *const req);
    void parse_connection(struct req *const req);
    
    void parse_content(std::string_view raw_content, req *const req, const struct config &config);
    
    template<typename T>
    std::vector<T> parse_field_list(req*const req, const std::string &field_name, err_builder*const err_handler)
    {
        /* relies on move semantics for copyless vector return. */
        std::vector<T> field_list;

        if (!req->fields.contains(field_name)) {
            return field_list;
        }

        std::vector<std::string> parsed_list;
        auto list_ptr = &parsed_list;
        utils::parse_list(req->fields[field_name], list_ptr);
        if (list_ptr == nullptr) {
            goto bad_req_err;
        }

        for (std::string &token : parsed_list) {
            T val = static_cast<T>(http_keyword_map::keyword_val(token));
            if (val == invalid_encoding) {
                goto t_err;
            }
            field_list.push_back(val);
        }
        return field_list;

    bad_req_err:
        req->err = &bad_req_handler;
        return field_list;
    t_err:
        req->err = err_handler;
        return field_list;
    }
}
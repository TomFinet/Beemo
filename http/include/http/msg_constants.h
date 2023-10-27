#pragma once

namespace http {

constexpr auto &connection_header = "connection";
constexpr auto &encoding_header = "transfer-encoding";
constexpr auto &host_header = "host";
constexpr auto &content_type_header = "content-type";

enum method_t {
    get, post, options, head, put,
    del, trace, connect, extension 
};

enum conn_t {
    keep_alive, close 
};

enum encoding_t {
    chunked, identity, gzip, compress, deflate 
};

enum content_type_t {
    html, json
};

enum target_form_t {
    absolute, authority, asterik, origin
};

struct version_t {
    short major;
    short minor;
};

typedef std::string token_t;
typedef std::vector<token_t> line_t;

const int status_code_len = 4;
const int reason_len = 20;

const char ok[status_code_len] = "200";
const char created[status_code_len] = "201";
const char accepted[status_code_len] = "202";
const char no_content[status_code_len] = "204";

const char multi_choices[status_code_len] = "300";
const char moved_permanently[status_code_len] = "301";
const char found[status_code_len] = "302";

const char bad_req[status_code_len] = "400";
const char unauthorised[status_code_len] = "401";
const char forbidden[status_code_len] = "403";
const char not_found[status_code_len] = "404";
const char req_timeout[status_code_len] = "408";
const char unsupported_media_type[status_code_len] = "415";
const char upgrade_required[status_code_len] = "426";

const char internal_err[status_code_len] = "500";
const char not_implemented[status_code_len] = "501";
const char bad_gateway[status_code_len] = "502";
const char service_unavailable[status_code_len] = "503";

}
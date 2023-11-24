#pragma once


namespace http
{

    constexpr auto &ok = "200";
    constexpr auto &created = "201";
    constexpr auto &accepted = "202";
    constexpr auto &no_content = "204";

    constexpr auto &multi_choices = "300";
    constexpr auto &moved_permanently = "301";
    constexpr auto &found = "302";

    //constexpr auto &bad_req = "400";
    constexpr auto &unauthorised = "401";
    constexpr auto &forbidden = "403";
    constexpr auto &not_found = "404";
    constexpr auto &req_timeout = "408";
    constexpr auto &unsupported_media_type = "415";
    constexpr auto &upgrade_required = "426";

    constexpr auto &internal_err = "500";
    constexpr auto &not_implemented = "501";
    constexpr auto &bad_gateway = "502";
    constexpr auto &service_unavailable = "503";

    constexpr auto &host_header = "host";
    constexpr auto &content_length_token = "content-length";
    constexpr auto &transfer_encoding_token = "transfer-encoding";
    constexpr auto &content_type_token = "content-type";
    constexpr auto &content_encoding_token = "content-encoding";
    constexpr auto &charset_token = "charset";

}
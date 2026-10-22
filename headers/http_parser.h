#pragma once

#include <iostream>
#include <string>

#include "socket_wrapper.h"

struct HTTP_req{
    socket_wrapper& fd;
    std::string method{};
    std::string path{};
    std::string protocol{};
};

class http_parser{
public:
    static int parse_address(const std::string& address, 
        std::string& ipv4, std::string& port);
    static void parse_request(HTTP_req& req, const std::string& buff);
};
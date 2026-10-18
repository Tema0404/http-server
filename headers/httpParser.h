#pragma once

#include <iostream>
#include <string>

struct HTTP_req{
    std::string method{};
    std::string path{};
    std::string protocol{};
};

class HTTP_parser{
public:
    static int parse_address(const std::string& address, 
        std::string& ipv4, std::string& port);
    static void parse_request(HTTP_req& req, std::string& buff);
};
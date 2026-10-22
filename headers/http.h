#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include <unordered_map>
#include <fstream>
#include <memory>
#include <sstream>

#include "net.h"

class HTTP_server{
    std::unique_ptr<net_server> server;
    std::unordered_map<std::string, std::function<void(HTTP_req&)>> table;

    int switch_http(HTTP_req& req);
    int send_response(socket_wrapper& socket, const int status, const std::string description, 
        const std::string content_type, const std::string& body = "");
    int handle_client(socket_wrapper& sock);
public:
    HTTP_server(const std::string address);
    HTTP_server(const HTTP_server&) = delete;

    void parsehtml_http(socket_wrapper& socket, const std::string& filename);
    void handle_http(const std::string path, 
        const std::function<void (HTTP_req&)>handle);
    int listen_http();
};
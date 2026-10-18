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
    std::unique_ptr<Net_server> server;
    socket_wrapper client_socket;
    size_t size = 0;
    std::unordered_map<std::string, std::function<void(HTTP_req*)>> table;

    int switch_http(HTTP_req* req);
    void page404_html();
    int send_response(const int status, const std::string description, 
        const std::string content_type, const std::string& body = "");
public:
    HTTP_server(const std::string address);
    HTTP_server(HTTP_server&) = delete;

    void parsehtml_http(const std::string& filename);
    void handle_http(const std::string path, 
        const std::function<void (HTTP_req*)>handle);
    int listen_http();
};
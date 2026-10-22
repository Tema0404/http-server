#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <memory>
#include <stdexcept>
#include <vector>
#include <unordered_map>

#include "http_parser.h"
#include "socket_wrapper.h"
#include "epoll_wrapper.h"

class net{
protected:
    socket_wrapper connection;
public:
    int send_net(socket_wrapper& client_sock, const std::string& buff);
    int recv_net(socket_wrapper& client_sock, std::string& answer);
};

class net_server: public net{
    static constexpr int MAX_EVENTS = 64;
    epoll_wrapper epoll_wrap;
    std::unordered_map<int, socket_wrapper> connections;
public:
    net_server(const std::string address);
    net_server(const net_server&) = delete;
    net_server& operator=(const net_server&) = delete;
    std::vector<int> wait_ready();
    socket_wrapper& get_conn(const int fd);
    int close_conn(const int fd);
};
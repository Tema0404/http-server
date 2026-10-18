#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <memory>
#include <stdexcept>
#include "httpParser.h"
#include "socket_wrapper.h"

class Net{
protected:
    socket_wrapper connection;
    static constexpr int MAX_BUFF = 256;
public:
    int send_net(socket_wrapper& client_sock, const std::string& buff);
    int recv_net(socket_wrapper& client_sock, std::string& answer);
};

class Net_client: Net{
public:
    Net_client(const std::string server_address);
};

class Net_server: public Net{
public:
    Net_server(const std::string address);
    int listen_net();
    int accept_net();
};
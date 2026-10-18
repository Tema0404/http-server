#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cerrno>
#include <cstring>


class socket_wrapper{
    int fd = -1;
public:
    explicit socket_wrapper(int fd);
    socket_wrapper() = default;
    socket_wrapper(const socket_wrapper&) = delete;
    socket_wrapper& operator=(const socket_wrapper&) = delete;

    socket_wrapper(const int domain, const int type, const int protocol);
    ~socket_wrapper();
    socket_wrapper(socket_wrapper&& socket) noexcept;
    socket_wrapper& operator=(socket_wrapper&& socket) noexcept;

    int set_opt(const int level, const int optname, 
        const void* optval, const socklen_t optlen);
    int bind_socket(const std::string& address, const int port);
    int connect_socket(const std::string& address, const int port);
    int listen_socket(const int backlog);
    int accept_socket(struct sockaddr* address, socklen_t* address_len);
    ssize_t send_to_socket(const char* buff, const size_t size, const int flags = MSG_NOSIGNAL);
    int recv_from_socket(std::string& buff, const size_t size, const int flags = 0);

    const int get_fd() const;
};
#pragma once

#include <sys/epoll.h>
#include <unistd.h>
#include <stdexcept>
#include <memory>

#include "socket_wrapper.h"

class epoll_wrapper{
    int epoll_fd = -1;
public:
    epoll_wrapper() = default;
    epoll_wrapper(socket_wrapper& listener);
    epoll_wrapper(const epoll_wrapper&) = delete;
    epoll_wrapper(epoll_wrapper&&) noexcept;
    epoll_wrapper& operator=(epoll_wrapper&&) noexcept;
    ~epoll_wrapper();

    int add_fd(socket_wrapper& new_connection, struct epoll_event *event);
    int add_fd(int new_connection, struct epoll_event *event);
    int wait(struct epoll_event *events, int max_events, int timeout = -1);
};
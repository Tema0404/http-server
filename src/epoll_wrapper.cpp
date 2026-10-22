#include "epoll_wrapper.h"

epoll_wrapper::epoll_wrapper(socket_wrapper& listener){
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if(epoll_fd < 0){
        throw std::runtime_error("error create epoll");
    }

    struct epoll_event event{};
    event.data.fd = listener.get_fd();
    event.events = EPOLLIN;
    if(this->add_fd(listener, &event) == -1){
        close(epoll_fd);
        throw std::logic_error("incorrect listen socket");
    }
}
epoll_wrapper::epoll_wrapper(epoll_wrapper &&right_epoll) noexcept{
    this->epoll_fd = right_epoll.epoll_fd;
    right_epoll.epoll_fd = -1;
}

epoll_wrapper& epoll_wrapper::operator=(epoll_wrapper &&right_epoll) noexcept{
    if(this == &right_epoll){
        return *this;
    }

    close(epoll_fd);
    this->epoll_fd = right_epoll.epoll_fd;
    right_epoll.epoll_fd = -1; 
    return *this;
}

epoll_wrapper::~epoll_wrapper(){
    if(epoll_fd >= 0){
        close(epoll_fd);
    }
}

int epoll_wrapper::add_fd(socket_wrapper& new_connection, struct epoll_event *event){
    return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_connection.get_fd(), event);
}

int epoll_wrapper::add_fd(int new_connection, struct epoll_event *event){
    return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_connection, event);
}

int epoll_wrapper::wait(struct epoll_event *events, int max_events, int timeout){
    return epoll_wait(epoll_fd, events, max_events, timeout);
}
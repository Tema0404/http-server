#include "socket_wrapper.h"

socket_wrapper::socket_wrapper(int fd): fd(fd) {
    if(fd < 0)
        throw std::runtime_error("incorrect file descryptor");
}
socket_wrapper::socket_wrapper(int domain, int type, int protocol){
    fd = socket(domain, type, protocol);
    if(fd < 0)
        throw std::runtime_error("incorrect file descryptor");
}
socket_wrapper::~socket_wrapper(){
    if(fd >= 0){
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
}
socket_wrapper::socket_wrapper(socket_wrapper&& socket) noexcept{
    if(this == &socket)
        return;
    fd = socket.fd;
    socket.fd = -1;
}
socket_wrapper& socket_wrapper::operator=(socket_wrapper&& socket) noexcept{
    if(this == &socket)
        return *this;

    close(fd);

    fd = socket.fd;
    socket.fd = -1;
    return *this;   
}

int socket_wrapper::set_opt(const int level, const int optname, 
        const void* optval, const socklen_t optlen){
    return setsockopt(fd, level, optname, optval, optlen);
}

int socket_wrapper::bind_socket(const std::string& address, const int port){
    // Связывание порта с сокетом

    if(port < 0 || address.size() < 7)
        return -1;

    struct sockaddr_in addr_in{};
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = inet_addr(address.c_str());
    if(bind(fd, (struct sockaddr*)&addr_in, sizeof(addr_in)) != 0){
        return -2;
    }
    return 0;
}

int socket_wrapper::connect_socket(const std::string& address, const int port){
    // подключение к серверу

    if(port < 0 || address.size() < 7)
        return -1;

    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = inet_addr(address.c_str());

    if(connect(fd, (struct sockaddr*)&addr_in, sizeof(addr_in)) != 0)
        return -2;
    return 0;
}

int socket_wrapper::listen_socket(const int backlog){
    return listen(fd, backlog);
}

int socket_wrapper::accept_socket(struct sockaddr* address, socklen_t* address_len){
    return accept(fd, address, address_len);
}

ssize_t socket_wrapper::send_to_socket(const char* buff, const size_t size, 
        const int flags){
    return send(fd, buff, size, flags);
}

int socket_wrapper::recv_from_socket(std::string& buff, 
        const size_t size, const int flags){
    buff.resize(size);
    int n = recv(fd, buff.data(), size, flags);
    if(n < 0){
        buff.clear();
        std::cerr << "recv failed: " << std::strerror(errno) << 
            "(errno=" << errno << ")\n"; 
    }
    else
        buff.resize(n);
    return n;
}

const int socket_wrapper::get_fd() const{
    return fd;
}

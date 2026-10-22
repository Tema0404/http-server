#include "net.h"

int net::send_net(socket_wrapper& client_sock, const std::string& buff){
    size_t size = buff.size();
    size_t total_sent = 0;  
    while(total_sent < size){
        ssize_t sent = client_sock.send_to_socket(buff.data() + total_sent, 
            size - total_sent);
            if(sent == 0)
                return -1;
            else if(sent == -1)
                return -2;
        total_sent += sent;
    }
    return 0;
}

int net::recv_net(socket_wrapper& client_sock, std::string& answer){
    if(answer.size() == 0){
        std::cerr << "incorrect buffer for recv";
        return -1;
    }
    int res = client_sock.recv_from_socket(answer, answer.size());
    return res;
}

net_server::net_server(const std::string address){

    // Создание сокета
    socket_wrapper new_listener(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    connection = std::move(new_listener);
    if(connection.get_fd() < 0)
        throw std::logic_error("socket error");
    
    // Установка reuseaddr
    int optval = 1;
    if(connection.set_opt(SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        throw std::logic_error("reuseaddr error");

    // Парсинг адреса
    std::string ipv4;
    std::string temp_port;
    if(http_parser::parse_address(address, ipv4, temp_port) != 0)
        throw std::logic_error("parse error");

    int port = stoi(temp_port);
    // биндинг
    if(connection.bind_socket(ipv4, port) != 0)
        throw std::logic_error("bind error");
        
    if(connection.listen_socket(SOMAXCONN) != 0)
        throw std::logic_error("listen error");

    // Добавление в epoll
    epoll_wrapper new_epoll(connection);
    epoll_wrap = std::move(new_epoll);
}

std::vector<int> net_server::wait_ready(){
    struct epoll_event events[MAX_EVENTS]; 

    int res_wait = epoll_wrap.wait(events, MAX_EVENTS);

    if(res_wait < 0){
        return {};
    }
    std::vector<int> vector_ready;
    vector_ready.reserve(res_wait);
    for(int i = 0; i < res_wait; ++i){
        int fd = events[i].data.fd;
        if(fd == connection.get_fd()){
            int new_socket = connection.accept_socket(0, 0); 
            std::cerr << "\naccepted fd=" << new_socket << "\n";
            if(new_socket >= 0){
                socket_wrapper sock(new_socket);
                struct epoll_event event{};
                event.events = EPOLLIN;
                event.data.fd = new_socket;
                if(epoll_wrap.add_fd(new_socket, &event) == 0){
                    connections.emplace(new_socket, std::move(sock));
                }
            }
        }
        else
            vector_ready.emplace_back(fd);
    }
    return vector_ready;
}

socket_wrapper& net_server::get_conn(const int fd){
    return connections.at(fd);
}

int net_server::close_conn(const int fd){
    int erase_res = connections.erase(fd);
    return erase_res == 1 ? 0 : -1;
}
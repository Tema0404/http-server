#include "net.h"

int Net::send_net(socket_wrapper& client_sock, const std::string& buff){
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

int Net::recv_net(socket_wrapper& client_sock, std::string& answer){
    std::string buff;
    int res = client_sock.recv_from_socket(buff, MAX_BUFF);
    answer = buff;
    return res;
}

Net_client::Net_client(const std::string server_address){

    // Создание сокета
    socket_wrapper new_connection(AF_INET, SOCK_STREAM, 0);
    this->connection = std::move(new_connection);
    if(connection.get_fd() < 0)
        throw std::logic_error("error connection");

    // Парсинг адреса
    std::string ipv4;
    std::string temp_port;

    if(HTTP_parser::parse_address(server_address, ipv4, temp_port) != 0)
        throw std::logic_error("error parse");

    int port = stoi(temp_port);
    // подключение к серверу
    if(connection.connect_socket(ipv4, port) !=0)
        throw std::logic_error("error connect");
}

Net_server::Net_server(const std::string address){

    // Создание сокета
    socket_wrapper new_listener(AF_INET, SOCK_STREAM, 0);
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
    if(HTTP_parser::parse_address(address, ipv4, temp_port) != 0)
        throw std::logic_error("parse error");

    int port = stoi(temp_port);
    // биндинг
    if(connection.bind_socket(ipv4, port) != 0)
        throw std::logic_error("bind error");
}

int Net_server::listen_net(){
    // Прослушивание порта
    if(connection.listen_socket(SOMAXCONN)){
        return -1;
    }

    return 0;
}

int Net_server::accept_net(){
    int new_socket = connection.accept_socket(0, 0);
    return std::move(new_socket);
}
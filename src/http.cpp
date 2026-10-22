#include "http.h"

int HTTP_server::switch_http(HTTP_req& req){
    if(!table.contains(req.path)){
        if(req.path.size() == 0){
            send_response(req.fd, 404, "Not Found", 
                "text/plain", "Not Found");
            return -1;
        }
        size_t pos_slash = req.path.find('/', 1);
        if(pos_slash == std::string::npos){
            send_response(req.fd, 404, "Not Found", 
                "text/plain", "Not Found");
            return -2;
        }
        std::string current_path = req.path.substr(0, pos_slash);
        if(!table.contains(current_path)){
            send_response(req.fd, 404, "Not Found", 
                "text/plain", "Not Found");
            return -3;
        }

        table[current_path](req);
        return 0;   
    }
    table[req.path](req);
    return 0;
}

int HTTP_server::send_response(socket_wrapper& socket, const int status, const std::string description, 
        const std::string content_type, const std::string& body){
    size_t body_size = body.size();
    std::ostringstream oss;

    oss << "HTTP/1.1 " << status << " " << description << "\r\n";
    if(body_size != 0){
        oss << "Content-Type: " + content_type + "\r\n";
    }
    oss << "Content-Length: " + std::to_string(body_size) + "\r\n" 
        << "Connection: close\r\n\r\n";

    std::string answer = oss.str(); 


    if(server->send_net(socket, answer) != 0){
        return -1;
    }
    if(body_size != 0){
        if(server->send_net(socket, body) != 0) 
            return -2;
    }

    return 0;
}

int HTTP_server::handle_client(socket_wrapper& sock){
    const int BUFF_SIZE = 2048;
    HTTP_req req(sock);
    std::string buffer;
    buffer.resize(BUFF_SIZE);
    int length = server->recv_net(sock, buffer);

    if(length <= 0)
        return -1;
    http_parser::parse_request(req, buffer);

    switch_http(req);
    return 0;
}

HTTP_server::HTTP_server(const std::string address){
    server = std::make_unique<net_server>(address);
}

void HTTP_server::parsehtml_http(socket_wrapper& socket , const std::string& filename){
    std::ifstream file(filename, std::ios::binary);

    if(!file){
        send_response(socket, 404, "Not Found", "text/plain", "Not Found");
    }
    else{
        std::string body((std::istreambuf_iterator<char>(file)), 
            std::istreambuf_iterator<char>()); 
        send_response(socket, 200, "OK", "text/html", body);
    }
}

void HTTP_server::handle_http(const std::string path, 
        const std::function<void (HTTP_req&)>handle){
    table[path] = handle;
}

int HTTP_server::listen_http(){
    while(true){
        std::vector<int> clients_sockets = server->wait_ready();
        size_t ready_size = clients_sockets.size();

        for(int i = 0; i < ready_size; ++i){
            try{
                int handle_client_res = handle_client(
                    server->get_conn(clients_sockets[i]));   
            }
            catch(const std::exception& ex){
                std::cerr << "client error: " << ex.what() << '\n';
            }
            
            server->close_conn(clients_sockets[i]);
        }
    }
    return 0;
}


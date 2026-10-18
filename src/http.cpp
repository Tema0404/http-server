#include "http.h"


void HTTP_server::parsehtml_http(const std::string& filename){
    std::ifstream file(filename, std::ios::binary);

    if(!file){
        send_response(404, "Not Found", "Not Found");
    }
    else{
        std::string body((std::istreambuf_iterator<char>(file)), 
            std::istreambuf_iterator<char>()); 
        send_response(200, "Ok", "text/html", body);
    }
}

int HTTP_server::send_response(const int status, const std::string description, 
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


    if(server->send_net(client_socket, answer) != 0){
        return -1;
    }
    if(body_size != 0){
        if(server->send_net(client_socket, body) != 0) 
            return -2;
    }

    return 0;
}

HTTP_server::HTTP_server(const std::string address){
    server = std::make_unique<Net_server>(address);
}


void HTTP_server::handle_http(const std::string path, 
        const std::function<void (HTTP_req*)>handle){
    table[path] = handle;
}
int HTTP_server::listen_http(){
    int listener = server->listen_net();
    if(listener < 0)
        return 1;
    while(true){
        int new_socket = server->accept_net();
        client_socket = socket_wrapper(new_socket);
        if(client_socket.get_fd() < 0)
            continue;
        HTTP_req req;

        std::string buffer;
        buffer.resize(2048);
        int length = server->recv_net(client_socket, buffer);
        if(length < 0)
            continue;
        HTTP_parser::parse_request(req, buffer);
        
        switch_http(&req);
    }
    return 0;
}

int HTTP_server::switch_http(HTTP_req* req){
    if(!table.contains(req->path)){
        if(req->path.size() == 0){
            page404_html();
            return -1;
        }
        int pos_slash = req->path.find('/', 1);
        if(pos_slash == std::string::npos){
            page404_html();
            return -2;
        }
        std::string current_path = req->path.substr(0, pos_slash);
        if(!table.contains(current_path)){
            page404_html();
            return -3;
        }

        table[current_path];
        return 0;   
    }
    table[req->path](req);
    return 0;
}

void HTTP_server::page404_html(){
    std::string header = "HTTP/1.1 404 Not Found\r\n\r\n"
                        "not found";
    server->send_net(client_socket, header);
}
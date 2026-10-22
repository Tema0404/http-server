#include "http_parser.h"


int http_parser::parse_address(const std::string& address, 
    std::string& ipv4, std::string& port){

    size_t find_colon = address.find(':');
    if(find_colon == std::string::npos)
        return -1;
    else if(find_colon > 15)
        return -2;
    
    ipv4 = address.substr(0, find_colon);
    
    port = address.substr(find_colon + 1);
    if(port.length() > 5)
        return -3;
    try{
        int port_int = std::stoi(port);
        if(port_int < 0 || port_int > 65535)
            throw std::logic_error("incorrect port number");
    }
    catch(std::exception& ex){
        std::cerr << ex.what() << '\n';
        return -4;
    }
    return 0;
}

void http_parser::parse_request(HTTP_req& req, const std::string& buff){
    std::string temp_buff = buff.substr(0, buff.find("\r\n"));
    std::cout << temp_buff << '\n';
    size_t pos_f_space = temp_buff.find(' ');
    req.method = temp_buff.substr(0, pos_f_space);

    size_t pos_s_space = temp_buff.find(' ', pos_f_space + 1);
    req.path = temp_buff.substr(pos_f_space + 1, pos_s_space - pos_f_space - 1);

    req.protocol = temp_buff.substr(pos_s_space + 1);
    return;
}
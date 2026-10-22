#include "http.h"
#include <memory>
#include <filesystem>

int main(){
    std::filesystem::path root = std::filesystem::path(PROJECT_ROOT) / "html_pages";

    auto server = std::make_unique<HTTP_server>("127.0.0.1:7545");
    server->handle_http("/", [&](const HTTP_req& req){
        server->parsehtml_http(req.fd, root / "index.html");
    });
    server->handle_http("/about", [&](const HTTP_req& req){
        server->parsehtml_http(req.fd, root / "about.html");
    });

    server->listen_http();
    return 0;
}
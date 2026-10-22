cmake_minimum_required(VERSION 3.16)

project(server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server 
    src/main.cpp
    src/http.cpp
    src/http_parser.cpp
    src/net.cpp
    src/socket_wrapper.cpp
    src/epoll_wrapper.cpp
)
target_include_directories(server PRIVATE headers)
target_compile_definitions(server PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

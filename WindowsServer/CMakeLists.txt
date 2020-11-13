cmake_minimum_required(VERSION 3.5)

project(Server)

set(CMAKE_CXX_STANDARD 14)

add_executable(Server server.cpp)

target_link_libraries(Server ws2_32)
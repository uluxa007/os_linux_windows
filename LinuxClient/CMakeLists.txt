cmake_minimum_required(VERSION 3.9)
project(Client)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(test)

add_executable(Client main.cpp client.cpp client.h)


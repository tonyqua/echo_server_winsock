cmake_minimum_required(VERSION 3.31.6)
project(echo_server_winsock)

set(CMAKE_CXX_STANDARD 17)

add_executable(echo_server_winsock serverwin.cpp
        clientwin.cpp)
target_link_libraries(echo_server_winsock ws2_32)
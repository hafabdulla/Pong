cmake_minimum_required(VERSION 3.28)
project(Pong)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2 COMPONENTS graphics audio REQUIRED)

add_executable(Pong main.cpp
        main.cpp)

target_link_libraries(Pong sfml-graphics sfml-audio)

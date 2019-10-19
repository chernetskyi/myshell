cmake_minimum_required(VERSION 3.14)
project(myshell)

set(CMAKE_C_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(./inc)

add_executable(mycat src/mycat.c inc/mycat.h)


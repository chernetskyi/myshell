cmake_minimum_required(VERSION 3.14)
project(myshell)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Boost REQUIRED COMPONENTS system filesystem program_options)
include_directories(./inc ${Boost_INCLUDE_DIRS})

add_executable(mycat src/mycat.c inc/mycat.h)

add_executable(myshell src/myshell.cpp inc/myshell.h src/builtins.cpp inc/builtins.h src/helpers.cpp inc/helpers.h)
target_link_libraries(myshell readline ${Boost_LIBRARIES})

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

add_executable(myls src/myls.cpp inc/myls.h)
target_link_libraries(myls ${Boost_LIBRARIES})

add_executable(mymkdir src/mymkdir.cpp)
target_link_libraries(mymkdir ${Boost_LIBRARIES})

add_executable(myrm src/myrm.cpp)
target_link_libraries(myrm ${Boost_LIBRARIES})

add_executable(mymv src/mymv.cpp)
target_link_libraries(mymv ${Boost_LIBRARIES})

add_executable(myshell src/myshell.cpp inc/myshell.h src/builtins.cpp inc/builtins.h src/command.cpp inc/command.h)
target_link_libraries(myshell readline ${Boost_LIBRARIES})

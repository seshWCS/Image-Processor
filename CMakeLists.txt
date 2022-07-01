cmake_minimum_required(VERSION 3.20)
project(photo_project)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(photo_project
        filter.cpp
        filter.h
        image.cpp
        image.h
        main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

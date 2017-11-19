cmake_minimum_required(VERSION 2.8.9)
project(tsp)

set(CMAKE_BUILD_TYPE Debug)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")
endif()

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(tsp ${SOURCES})

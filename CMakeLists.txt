cmake_minimum_required(VERSION 3.1)
project(tsp)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -pedantic")

file(GLOB SOURCES "src/*.cpp")

add_executable(tsp ${SOURCES})

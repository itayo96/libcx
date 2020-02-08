cmake_minimum_required(VERSION 2.8.9)

project(ftp-server)

include_directories(include)
include_directories(shared)

set (CMAKE_CXX_FLAGS "-pthread")

file(GLOB SOURCES src/*.cpp)

add_executable(ftp-server ${SOURCES})
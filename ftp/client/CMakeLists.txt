cmake_minimum_required(VERSION 2.8.9)

project(ftp-client)

include_directories(include)
include_directories(shared)

file(GLOB SOURCES src/*.cpp)

add_executable(client ${SOURCES})
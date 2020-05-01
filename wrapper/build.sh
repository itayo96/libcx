#!/bin/bash

echo "Building wrapper..."

# check if 'bin' directory exists and create it if needed
if [ ! -d "bin" ]
then
    mkdir bin
fi

#g++ src/libcx.cpp src/heap_management.cpp src/file_operations.cpp -fPIC -shared -o bin/libcx.so -ldl
g++ src/libcx.cpp src/libcx.h src/message_builder.h src/file_operations.cpp -fPIC -shared -o bin/libcx.so -ldl

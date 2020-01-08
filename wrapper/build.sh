#!/bin/bash

# check if 'bin' directory exists and create it if needed
if [ ! -d "bin" ]
then
    mkdir bin
fi

g++ src/libcx.cpp -fPIC -shared -o bin/libcx.so -ldl
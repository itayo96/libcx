#!/bin/bash

# check if 'bin' directory exists and create it if needed
if [ ! -d "bin" ]
then
    mkdir bin
fi

gcc src/libcx.c -fPIC -shared -o bin/libcx.so -ldl
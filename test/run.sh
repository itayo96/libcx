#!/bin/bash

if [ ! -d "env" ]; then
    mkdir env
fi

cd example
./build.sh

cd ../
cp example/bin/example env/example

cd env
LD_PRELOAD=$(pwd)/libcx.so ./example
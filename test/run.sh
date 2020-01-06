#!/bin/bash

cd example
./build.sh

cd ../
cp example/bin/example env/example

cd env
LD_PRELOAD=libcx.so ./example

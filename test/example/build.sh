#!/bin/bash
if [ ! -d "bin" ]; then
    mkdir bin
fi

gcc src/main.c -o bin/example
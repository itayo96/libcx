#!/bin/bash

gcc src/libcx.c -fPIC -shared -o bin/libcx.so -ldl
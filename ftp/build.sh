#!/bin/sh

# setup colors
RED='\033[0;31m'
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
NC='\033[0m'

# build and install the client
cd client/build 

echo "${YELLOW} > Generating makefile for client ... ${NC}"
cmake ..
if [ $? -eq 0 ]; then
    echo "${GREEN} Successfully generated makefile for client ${NC}"
else 
    echo "${RED} Failed to generate makefile for client ${NC}"
    exit
fi 

echo "${YELLOW} > Building SFTP client ... ${NC}"
make
if [ $? -eq 0 ]; then
    echo "${GREEN} Successfully built SFTP client ${NC}"
else 
    echo "${RED} Failed to build SFTP client ${NC}"
    exit
fi

echo "${YELLOW} > Installing SFTP client ... ${NC}"
cp client ../../build

# build and install the server
cd ../../server/build

echo "${YELLOW} > Generating makefile for server ... ${NC}"
cmake ..
if [ $? -eq 0 ]; then
    echo "${GREEN} Successfully generated makefile for server ${NC}"
else 
    echo "${RED} Failed to generate makefile for server ${NC}"
    exit
fi 

echo "${YELLOW} > Building SFTP server ... ${NC}"
make
if [ $? -eq 0 ]; then
    echo "${GREEN} Successfully built SFTP server ${NC}"
else 
    echo "${RED} Failed to build SFTP server ${NC}"
    exit
fi

echo "${YELLOW} > Installing SFTP server ... ${NC}"
cp server ../../build

echo "${GREEN} > Done! ${NC}"
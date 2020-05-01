#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "libcx.h"
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

// The externed object
initializer libcx;

std::string home_relative_path(const std::string& path)
{
    return std::string(getenv("HOME")) + path;
}

initializer::initializer()
{
    printf("Starting with version 1.0\n");
    pid = getpid();

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1)
    {
        printf("error creating control socket - %d\n", errno);
        return;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    auto path = home_relative_path("/.libcx/daemon.uds");
    const char* sock_path = path.c_str();
    
    strncpy(addr.sun_path, sock_path, strlen(sock_path));

    printf("Connecting to %s\n", addr.sun_path);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        printf("error connecting control socket - %d\n", errno);
        return;
    }

    // This code actually runs before programs main and even before its static constructors
    printf("libcx main start pid = %d\n", static_cast<int>(pid)); 
}

void initializer::report(size_t message_size)
{
    // for (int i = 0; i < message_size; i++)
    // {
    //     printf("0x%02X, ", buffer[i]);
    // }
    // printf("\n");

    int actual_size = write(fd, buffer, message_size);
    if (actual_size != message_size)
    {
        printf("error sending connection message - %d\n", actual_size);
    }
}
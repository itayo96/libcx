#define _GNU_SOURCE

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

size_t initializer::write_to_supervisor(size_t size)
{
    int written = write(fd, buffer, size);

    if (written <= 0)
    {
        return 0;
    }

    return size_t(written);
}


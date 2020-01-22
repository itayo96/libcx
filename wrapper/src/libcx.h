#ifndef LIBCX_H
#define LIBCX_H

#include <dlfcn.h>
#include <stdint.h>
#include <sys/types.h>
#include "message_builder.h"

struct initializer
{
    static constexpr size_t MAX_MESSAGE_SIZE = 2000;
    uint8_t buffer[MAX_MESSAGE_SIZE];

    int fd;
    pid_t pid;

    initializer();
    void report(size_t message_size);
};

extern initializer libcx;

#endif // ifndef LIBCX

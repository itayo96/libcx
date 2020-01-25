#ifndef LIBCX_H
#define LIBCX_H

#include <dlfcn.h>
#include <stdint.h>
#include <sys/types.h>
#include "message_builder.h"
#include <stdio.h>

struct initializer
{
    static constexpr size_t MAX_MESSAGE_SIZE = 2000;
    uint8_t buffer[MAX_MESSAGE_SIZE];

    int fd;
    pid_t pid;

    initializer();

    size_t write_to_supervisor(size_t size);

    template<class ...Params>
    void report(ELibCall lib_call, Params ...params)
    {
        size_t message_size = message_builder::build_message(buffer, lib_call, pid, params...);

        // for (int i = 0; i < message_size; i++)
        //     printf("0x%02X, ", buffer[i]);
        // printf("\r\n");

        size_t actual_size = write_to_supervisor(message_size);

        if (actual_size != message_size)
        {
            printf("error sending connection message - %u\n", actual_size);
        }
    }
};

extern initializer libcx;

#endif // ifndef LIBCX

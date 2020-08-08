#ifndef LIBCX_H
#define LIBCX_H

#include <dlfcn.h>
#include <stdint.h>
#include <sys/types.h>
#include "message_builder.h"

struct initializer
{
    static constexpr size_t MAX_MESSAGE_SIZE = 2000;
    static constexpr size_t MAX_MESSAGES_IN_CACHE = 20;

    uint8_t buffer[MAX_MESSAGE_SIZE];

    int fd;
    pid_t pid;

    initializer();
    void report(size_t message_size);

private:

	void _send();

	size_t _cache_i;

	struct {
		size_t size;
		unsigned char data[MAX_MESSAGE_SIZE];
	} _cache[MAX_MESSAGES_IN_CACHE];
};

extern initializer libcx;

#endif // ifndef LIBCX

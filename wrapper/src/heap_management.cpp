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

static void * (*real_calloc)(size_t, size_t);

extern "C" void *calloc(size_t nmemb, size_t size)
{
    real_calloc = (decltype(real_calloc))dlsym(RTLD_NEXT, "calloc");

    puts("calloc2\n");

    void *return_value = real_calloc(nmemb, size);

    libcx.report(ELibCall::calloc, nmemb, size, return_value);

    return return_value;
}

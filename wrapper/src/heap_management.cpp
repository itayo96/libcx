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

static void *(*real_calloc)(size_t, size_t);
static void *(*real_malloc)(size_t size);
static void (*real_free)(void *ptr);
static void *(*real_memset)(void *s, int c, size_t n);

extern "C" void *malloc(size_t size)
{
    real_malloc = (decltype(real_malloc))dlsym(RTLD_NEXT, "malloc");
    //puts("malloc2");

    void *return_value = real_malloc(size);

    libcx.report(ELibCall::malloc, uint32_t(size), uint64_t(return_value));
    return return_value;
}

extern "C" void *calloc(size_t nmemb, size_t size)
{
    real_calloc = (decltype(real_calloc))dlsym(RTLD_NEXT, "calloc");

    puts("calloc2\n");

    void *return_value = real_calloc(nmemb, size);

    libcx.report(ELibCall::calloc, uint32_t(nmemb), uint32_t(size), uint64_t(return_value));

    return return_value;
}

extern "C" void free(void *ptr)
{
    real_free = (decltype(real_free))dlsym(RTLD_NEXT, "free");
    puts("free2\n");

    real_free(ptr);

    libcx.report(ELibCall::free, uint64_t(ptr));
}

extern "C" void *memset(void *s, int c, size_t n)
{
    // void* = size_t = 8, int = 4
    real_memset = (decltype(real_memset))dlsym(RTLD_NEXT, "memset");
    puts("memset2\n");
    
    void *return_value = real_memset(s, c, n);

    libcx.report(ELibCall::memset, uint64_t(s), uint8_t(c), uint32_t(n), uint64_t(return_value));

    return return_value;
}

#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

static void * (*real_calloc)(size_t, size_t);
int c = 0;

void *calloc(size_t a, size_t b)
{
    real_calloc = dlsym(RTLD_NEXT, "calloc");
    c++;

    puts("calloc\n");

    return real_calloc(a, b);
}

void _()
{
    void * addr = calloc(1, 10);
    free(addr);
}
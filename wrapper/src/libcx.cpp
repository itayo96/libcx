#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <unistd.h>
#include <sys/types.h>

static struct main
{
    main()
    {
        // This code actually runs before programs main and even before its static constructors
        printf("libcx main start pid = %d\n", static_cast<int>(getpid()));
    }
} _;

static void * (*real_calloc)(size_t, size_t);
int c = 0;

void *calloc(size_t a, size_t b)
{
    real_calloc = (decltype(real_calloc))dlsym(RTLD_NEXT, "calloc");
    c++;

    puts("calloc2\n");

    return real_calloc(a, b);
}
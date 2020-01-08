#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

extern "C"  void *dlsym(void *, const char *);

static struct main
{
    main()
    {
        printf("Called the main\n");
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
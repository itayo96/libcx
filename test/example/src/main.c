#include <stdio.h>
#include <stdlib.h>

void _()
{
    void * addr = calloc(1, 10);
    free(addr);
}

void main()
{
    for (int i = 0; i<5; i++) _();

    printf("Omer2\n");
}
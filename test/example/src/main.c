#include <stdio.h>
#include <stdlib.h>

void fu_horev()
{
    void * addr = calloc(1, 10);
    free(addr);
}

void main()
{
    printf("program main\n");

    for (int i = 0; i<5; i++) fu_horev();

    printf("Omer2\n");
}
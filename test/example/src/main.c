#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

void fu_horev()
{
    void * addr = calloc(1, 10);
    free(addr);
}

void hexdump(uint8_t *p, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        if (i % 16 == 0)
        {
            printf("\r\n");
        }

        printf("0x%02X, ", p[i]);
    }

    printf("\r\n");
}

void main()
{
    //printf("program main %d %d %d\n", sizeof(int), sizeof(void*), sizeof(size_t));

    // for (int i = 0; i<5; i++) fu_horev();

    // printf("Omer2\n");

    // sleep(1);

    // uint8_t *p = calloc(1, 10);
    // free(p);
    // p[0] = 4;

    // hexdump(p, 10);
}
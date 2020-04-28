#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void fu_horev()
{
    void * addr = calloc(1, 10);
    free(addr);
}

void main()
{
    printf("program main\n");

   // for (int i = 0; i<5; i++) fu_horev();
    FILE * f = fopen("kakashka.txt", "w");
    fclose (f);
    printf("Omer2\n");
    
    sleep(1);

    while(1);
}
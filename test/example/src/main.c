#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

FILE* g_log = NULL;


int main() {
	 g_log = fopen("log.txt", "a+");

	 if (g_log == NULL) {
	 	return 1;
	 }

	 fputs("Starting the program...\n", g_log);

	 fputs("End!\n", g_log);

	 fclose(g_log);

	 return 0;
}
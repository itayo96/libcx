#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ACTIONS_URL "https://raw.githubusercontent.com/itayo96/libcx/master/test/resources/actions"

FILE* g_log = NULL;


int main() {
	printf("%s\n", "Starting...");
	g_log = fopen("log.txt", "a+");

	if (g_log == NULL) {
		return 1;
	}

	fputs("Starting the program...\n", g_log);
	fputs("executing wget...\n", g_log);

	system("wget " ACTIONS_URL);
	fputs("downloaded\n", g_log);

	FILE* actions = fopen("actions", "r");

	if (actions == NULL)
	{
		fputs("Error opening the actions file\n", g_log);
		return 1;
	}

	fclose(actions);
	fclose(g_log);

	return 0;
}
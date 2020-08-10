#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ACTIONS_URL "https://raw.githubusercontent.com/itayo96/libcx/master/test/resources/actions2"

FILE* g_log = NULL;
FILE* g_devurandon = NULL;

#define FILE_SIZE 256
#define BUFFER_SIZE 16


int main() {
	printf("%s\n", "Starting...");
	g_log = fopen("log.txt", "a+");
	g_devurandon = fopen("/dev/urandom", "r");

	if (g_log == NULL || g_devurandon == NULL) {
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

	char filename[256];
	int count;
	int r;
	int a = 0;

	r = fscanf(actions, "%s %d", filename, &count);
	while (r != EOF) 
	{
		a++;
		r = fscanf(actions, "%s %d", filename, &count);
		fputs(filename, g_log);
		fputs("\n", g_log);

		FILE* f = fopen(filename, "w");

		int amount = rand() % FILE_SIZE;

		printf("--------------------------------\n");
		printf("%d: Writing %d bytes\n", a, amount);

		char data[BUFFER_SIZE];

		int i = 0;
		while (i + BUFFER_SIZE < amount) 
		{
			fread(data, BUFFER_SIZE, 1, g_devurandon);
			fwrite(data, BUFFER_SIZE, 1, f);
			i += BUFFER_SIZE;
		}

		int data_left = BUFFER_SIZE - i;
		fread(data, data_left, 1, g_devurandon);
		fwrite(data, data_left, 1, f);
		fclose(f);
	}

	fclose(actions);

	fclose(g_devurandon);
	fclose(g_log);

	return 0;
}
#include <stdlib.h>
#include <stdio.h>

char *usage = "usage: rename file file2\n";

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		fprintf(stderr, usage);
		exit(1);
	}
	if(rename(argv[1], argv[2]) == -1)
	{
		perror("remove failed");
		exit(1);
	}
	printf("Succeeded\n");
	exit(0);
}

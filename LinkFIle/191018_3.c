#include <stdlib.h>
#include <stdio.h>

char *usage = "usage: remove file\n";

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stderr, usage);
		exit(1);
	}

	if(remove(argv[1]) == -1)
	{
		perror("remove failed");
		exit(1);
	}

	printf("Succeeded\n");
	exit(0);
}

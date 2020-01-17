#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

char *usage = "usage: move file file2\n";

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		fprintf(stderr, usage);
		exit(1);
	}
	if( link(argv[1], argv[2]) == -1)
	{
		perror("link failed");
		exit(1);
	}
	if(unlink(argv[1]) == -1)
	{
		perror("unlink failed");
		unlink(argv[2]);
		exit(1);
	}

	printf("Succeeded\n");
	exit(0);
}

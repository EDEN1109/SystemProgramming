#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#define XPERM 0100

int main (int argc, char **argv)
{
	struct stat statbuf;

	for(int k = 1; k < argc; k++)
	{
		if(stat(argv[k], &statbuf) == -1)
		{
			fprintf(stderr, "addx: Couldn't stat %s\n", argv[k]);
			continue;
		}

		statbuf.st_mode |= XPERM;
		if(chmod(argv[k], statbuf.st_mode) == -1)
			fprintf(stderr, "addx: Couldn't change mode for %s\n", argv[k]);
	}
	exit(0);
}

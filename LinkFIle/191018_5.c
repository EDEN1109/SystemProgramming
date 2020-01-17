#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static short octarray[9] = { 0400, 0200, 0100,
									  0040, 0020, 0010,
									  0004, 0002, 0001 };

static char perm[10] ="rwxrwxrwx";

char *usage = "usage: filedata file\n";

int filedata(const char *pathname)
{
	struct stat statbuf;
	char descrip[10];

	if(stat (pathname, &statbuf) == -1)
	{
		fprintf(stderr, "Couldn't stat %s\n", pathname);
		return -1;
	}

	for(int j = 0; j < 9; j++)
	{
		if(statbuf.st_mode & octarray[j])
			descrip[j] = perm[j];
		else
			descrip[j] = '-';
	}
	descrip[9] = '\0';

	printf("\nFile %s :\n", pathname);
	printf("Size %ld bytes\n", statbuf.st_size);
	printf("User-id %d, Group-id %d\n\n", statbuf.st_uid, statbuf.st_gid);
	printf("Permission: %s\n", descrip);
	return 0;
}

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stderr, usage);
		exit(1);
	}
	if(filedata(argv[1]) == -1)
	{
		perror("filedata failed");
		exit(1);
	}
	printf("Succeeded\n");
	exit(0);
}

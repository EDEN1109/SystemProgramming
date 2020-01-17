#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MSGSIZE 16

char *msg1 = "hello, world #1";
char *msg2 = "hello, world #2";
char *msg3 = "hello, world #3";

int main()
{
    char inbuf[MSGSIZE];
    int fd[2], j;
    pid_t pid;

    if(pipe(fd) == -1)
    {
        perror("pipe call");
        exit(1);
    }
    
    switch(pid = fork())
    {
        case -1:
            perror("fork error");
            exit(1);
        case 0:
            write(fd[1], msg1, MSGSIZE);
            write(fd[1], msg2, MSGSIZE);
            write(fd[1], msg3, MSGSIZE);
            break;
        default:
            for(j = 0; j < 3; j++)
            {
                read(fd[0], inbuf, MSGSIZE);
                printf("%s\n", inbuf);
            }
            wait(NULL);

    }

    exit(0);
}

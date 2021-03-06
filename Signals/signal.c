#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    void catchint(int);
    signal(SIGINT, catchint);
    printf("sleep call #1\n", sleep(4));
    printf("sleep call #2\n", sleep(4));
    printf("Exiting\n");
    exit(0);
}

void catchint(int signo) {
    printf("\nCATCHINT: signo = %d\n", signo);
    printf("CATCHINT: returning\n\n");
}

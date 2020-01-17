#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    static struct sigaction act;
    void catchint(int);
    act.sa_handler = catchint;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);
    printf("sleep call #1\n", sleep(4));
    printf("sleep call #2\n", sleep(4));
    printf("sleep call #3\n", sleep(4));
    printf("sleep call #4\n", sleep(4));
    printf("Exiting\n");
    exit(0);
}

void catchint(int signo) {
    printf("\nCATCHINT: signo = %d\n", signo);
    printf("CATCHINT: returning\n\n");
}

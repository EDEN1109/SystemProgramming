#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main() {
    static struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    printf("sleep call #1\n", sleep(4));
    printf("sleep call #2\n", sleep(4));
    printf("sleep call #3\n", sleep(4));
    printf("sleep call #4\n", sleep(4));
    printf("Exiting\n");
    exit(0);
}

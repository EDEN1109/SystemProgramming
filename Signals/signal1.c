#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int ntimes = 0;

int main() {
    pid_t pid;
    void p_action(int);
    static struct sigaction pact;

    pact.sa_handler = p_action;
    sigaction(SIGUSR1, &pact, NULL);
    printf("my pid = %d\n", getpid());
    printf("other pid = ");
    scanf("%d", &pid);
    
    for(;;)
    {
        pause();
        sleep(1);
        kill(pid, SIGUSR1);
    }
    
}

void p_action(int sig)
{
    printf("Parent caught signal #%d\n", ++ntimes);
}

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>

// 상황에 따른 상수 지정
#define EOL 1
#define ARG 2
#define AMPERSAND 3
#define SEMICOLON 4
#define REDIRECT 5

// 문자열 길이 지정
#define MAXARG 512
#define MAXBUF 512

// 백그라운드 포그라운드 상수 지정
#define FOREGROUND 0
#define BACKGROUND 1

static char inpbuf[MAXBUF],tokbuf[2*MAXBUF],
    *ptr = inpbuf,*tok = tokbuf;
int userin(char *p);
int gettok(char **outptr);
int inarg(char c);
int procline(void);
int runcommand(char **cline, int where, int pipe);
void join(char** com1, char** com2);
void getOrder(char** argo, char** com1, char** com2);
void toDoList(int sig);
void sigact(int isFore);

// 명령어 등 입력을 받아오는 함수
int userin(char *p)
{
    int c, count;
    ptr = inpbuf; // ptr이 inpbuf를 가리키게 한다.
    tok = tokbuf; // tok가 tokbuf를 가리키게 한다.
    printf("%s", p); // 명령어를 입력하라는 안내문을 띄운다.
    count = 0; // count를 0으로 초기화한다.

    while(1) // 반복하여 실행한다.
    {
        if((c = getchar()) == EOF) // 만일 입력받은 문자가 End Of File, 즉 Ctrl+Z등을 만났을 경우
            return EOF; // EOF를 반환한다.
        if(count < MAXBUF) // count가 아직 MAXBUF(최대 버퍼의 크기)보다 작다면
            inpbuf[count++] = c; // inpbuf에 입력받은 문자를 입력한뒤 count를 늘린다.
        if(c=='\n' && count<MAXBUF) // 입력받은 문자가 \n이고 count가 아직 MAXBUF(최대 버퍼의 크기)보다 작다면
        {
            inpbuf[count] = '\0'; // inpbuf의 마지막 문자를 \0로 처리한다.
            return count; // 문자열의 길이를 반환한다.
        }

		// 위의 조건에 해당되지 않고 입력받은 문자가 \n이라면
		// 문자열 길이를 넘어간 것이므로
        if(c=='\n') 
        {
			// 문자열이 너무 길다는 안내문을 띄운후 count를 0으로 초기화 한다.
            printf("smallsh : input line too long\n");
            count = 0;
            printf("%s", p); // 명령어를 입력하라는 안내문을 띄운다.
        }
    }
}

// 입력받은 문장을 알맞게 자르는 함수
// 문장을 자르고 ; & | 엔터 등 지정한 특수문자가 들어오면 type을 알맞게 변경한다.
// 문장이 들어온다면 type을 ARG로 지정한다.
int gettok(char **outptr)
{
    int type;

    *outptr = tok;

    // 만일 Tab이나 띄워쓰기라면 Tab이나 띄워쓰기가 아닐때까지 ++한다.
    while(*ptr==' ' || *ptr=='\t')
        ptr++;
    // Tab이나 띄워쓰기 바로 다음을 문장의 시작점으로 지정한다.
    *tok++ = *ptr;

	// 입력받은 내용에 맞게 type을 지정한다.
    switch(*ptr++){
        case '\n':
            type = EOL;
            break;
        case '&':
            type = AMPERSAND;
            break;
        case ';':
            type = SEMICOLON;
            break;
		case '|':
			type = REDIRECT;
			break;
        default:
            type = ARG;
            // 문장이 아닌 특수문자를 만날때까지 문자를 받는다.
            while(inarg(*ptr))
                 *tok++ = *ptr++;
            break;
    }

    // 문장의 끝에 문장의 끝임을 알리는 \0를 추가한다.
    *tok++ = '\0';

	// type를 반환한다.
    return type;
}

// 문장이 아닌 것을 의미하는 특수문자를 지정한다.
static char special[] = {' ', '\t', '&', ';', '\n', '\0', '|'};

int inarg(char c)
{
    char *wrk;
    for(wrk = special; *wrk; wrk++)
    {
        // 특수문자를 만나면 return 0한다.
        if(c==*wrk)
            return 0;
    }
    return 1;
}

// 시그널을 처리하기 위한 변수
static struct sigaction act;

void toDoList(int sig)
{
}

void sigact(int isFore) {
    // 만일 포그라운드 프로그램이라면
    if(isFore == FOREGROUND) {
        act.sa_handler = toDoList; // SIGINT가 들어오면 행할 행동을 적용한다.
        sigfillset(&(act.sa_mask));
        // SIGINT를 적용한다.
        sigaction(SIGINT, &act, NULL);
    }
    else if(isFore == BACKGROUND){
        act.sa_handler = SIG_IGN; // SIGINT와 SIGQUIT가 들오면 행할 행동을 무시하는 것으로 적용한다.
        sigfillset(&(act.sa_mask));
        // SIGINT와 SIGQUIT를 무시한다.
        sigaction(SIGINT, &act, NULL);
        sigaction(SIGQUIT, &act, NULL);
    }
}

// 입력받은 타입에 맞는 동작을 실행한다.
int procline(void) {
    char *arg[MAXARG+1];
    int toktype;
    int narg = 0;
    int type;
    int nP = 0;
     
    for(;;)
    {
        // 입력받은 문자열의 type를 받아온다.
        switch( toktype = gettok(&arg[narg])) {
            case ARG: // 문자열이면
            case REDIRECT: // PIPE( | )라면
                if(narg<MAXARG) // 마지막 문자열이 아니라면 ++해준다
                    narg++;
                if(toktype == REDIRECT)
                    nP = 1; // nP를 1로 설정한다.
                break;
            case EOL:
            case SEMICOLON: // 세미콜론( ; )이라면
            case AMPERSAND: // 엠퍼센트( & )이라면
                if(toktype == AMPERSAND) // 엠퍼센트라면
                    type = BACKGROUND; // 백그라운드로 지정한다.
                else // 아니라면
                    type = FOREGROUND; // 포그라라운드로 지정한다.
                if(narg != 0) // 첫번째 문자열이 아니라면
                {
                    arg[narg] = NULL; // 마지막 문자열을 null로 변경한다.
                    runcommand(arg, type, nP); // 명령문을 실행한다.
                    sigact(BACKGROUND); // 백그라운드로 설정하여 sigact를 실행한다.
                }
                if(toktype == EOL) // 만일 타입이 EOL이라면 종료한다.
                    return;
                narg = 0; // narg를 0으로 초기화한다.
                break;
        }
    }
}

// 명령문을 실행하는 함수.
int runcommand(char** cline, int where, int pipe) {
    pid_t pid;
    int status; // 자식 프로세스의 상태

    // 명령어에 logout이 있는지 찾는다. 있다면 프로그램을 끝마친다.
    if(strcmp(*cline, "logout") == 0)
        exit(EXIT_SUCCESS);

    // fork를 통해 프로세스를 복제하여 자식을 생성한다.
    switch(pid = fork()){
        case -1: // 실패하면 프로그램을 끝낸다.
            perror ("smallsh");
            return(-1);
        case 0:
            if(where == BACKGROUND) // 백그라운드 프로세스라면
                sigact(BACKGROUND); // 백그라운드로 설정하여 sigact를 실행한다.
            else
                sigact(FOREGROUND); // 포그라운드로 설정하여 sigact를 실행한다.
            if(pipe) { // 파이프 명령어라면
                // 명령어를 담을 변수를 NULL로 초기화한다.
                char* order1[20] = {NULL,};
                char* order2[20] = {NULL,};
                // 명령어를 추출하여 order1과 2에 넣는다.
                getOrder(cline, order1, order2);
                // order1과 2으로 join을 실행한다.
                join(order1, order2);
                exit(1);
            }
            // 명령어를 실행한다.
            execvp(*cline,cline);
            perror(*cline);
            exit(1);
    }

    // 백그라운드 프로그램은 pid를 출력한다.
    if(where == BACKGROUND)
    {
        printf("[Process id %d]\n",pid);
        return ( 0 );
    }

    // 자식의 상태를 받아온다.
    if(waitpid(pid,&status,0) == -1)
        return( -1);
    else
        return(status);
}

// pipe처리를 위한 함수
void join(char** com1, char** com2) {
    int p[2];
    // 명령어를 저장할 변수, NULL로 초기화한다.
    char* order1[20] = {NULL,};
    char* order2[20] = {NULL,};

    // 파이프에 실패했다면 오류문을 띄우고 종료한다.
    if(pipe(p) == -1) {
        perror("pipe call");
        exit(1);
    }

    switch(fork()) {
        case -1:
            perror("smallsh2");
            exit(1);
        case 0:
            // p[1]을 표준출력으로 한다.
            dup2(p[1], 1);

            // p[0]와 p[1]을 닫는다.
            close(p[0]);
            close(p[1]);

            // com1의 명령어를 실행한다.
            execvp(*com1, com1);
            perror("execvp1");
            exit(1);
        default:
            // p[0]을 표준입력으로 한다.
            dup2(p[0], 0);

            // p[0]와 p[1]을 닫는다.
            close(p[0]);
            close(p[1]);

            // com2의 명령어를 추출하여 order1 과 2에 넣는다.
            getOrder(com2, order1, order2);

            // order1와 2 중 하나라도 입력된 명령어가 없다면
            if(!order1[0] || !order2[0]) {
                // com2의 명령어를 실행한다.
                execvp(*com2, com2);
                perror("execvp2");
                exit(1);
            }
            else
                // 둘 다 입력된 명령어가 있다면 재귀한다.
                join(order1, order2);
    }
}

// 명령어를 추출하여 com1과 2에 저장하는 함수
void getOrder(char** argo, char** com1, char** com2) {
    // com1에 넣을지 com2에 넣을지 결정하는 변수
    int flag = 0;

    // *argo가 비어있지 않다면
    while(*argo!=NULL) {
        // **argo가 파이프를 나타내는 값이고 아직 플래그가 세워지지 않았다면
        if(**argo=='|' && flag==0) {
            flag = 1; // 플래그를 세운다.
            argo++; // 다음 문장으로 넘어간다.
        }
        if(flag == 0) // 플래그가 세워져있지 않다면
        {
            *com1 = *argo; // *argo의 문장을 *com1에 저장한다.
            com1++; // com1을 다음 문장을 넣기위해 ++한다.
        }
        else
        {
            *com2 = *argo; // *argo의 문장을 *com2에 저장한다.
            com2++; // com2을 다음 문장을 넣기위해 ++한다.
        }
        argo++; // 다음 문장으로 넘어간다.
    }

	// 문장의 끝을 알리기 위해 com1과 2의 마지막 문자를 NULL로 지정한다.
    *com1 = NULL;
    *com2 = NULL;
}

char *prompt = "Prompt> ";

int main() {
    sigact(BACKGROUND); // 백그라운드로 설정하여 sigact를 실행한다.
    while(userin(prompt) != EOF) // 입력받은 프롬프트(명령어)가 EOF가 아니라면
        procline(); // 함수를 실행
    return 0;
}

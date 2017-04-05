#include <stdio.h>
#include <unistd.h>
#include <signal.h>

# define T 5

int flag = T;

void sigalrm_handler (int sig) {
    if (--flag)
        printf("Hi...\n");
    else {
        printf("BYE\n");
        flag=T;     
    }

    alarm(1);
}

int main (void) {
    signal(SIGALRM, sigalrm_handler);   
    alarm(1);                         
    while (1);  
}


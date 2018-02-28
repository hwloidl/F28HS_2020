
// import the relevant header files
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

/* signal handler, i.e. the fct called when a signal is received */
void handler(int sig) 
{
    static int beeps = 0; /* NB */

    printf("BEEP %d\n", beeps+1);
    if (++beeps < 5)  
	alarm(1); /* Next SIGALRM will be delivered in 1 second */
    else {
	printf("BOOM!\n");
	exit(1);
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int main() {
    signal(SIGALRM, handler); /* install SIGALRM handler; see: man 2 signal */
    alarm(1); /* Next SIGALRM will be delivered in 1s; see: man 2 alarm */

    while (1) { /* nothing */ ; /* Signal handler returns control here each time */
    }
    exit(0);
}



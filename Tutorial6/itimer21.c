/*
Simple example code of using an itimer (interval timer) through Assembler and system calls.

Compile: gcc -save-temps -DASM  -o itimer21 itimer21.c
Run:     ./itimer21

From: http://www.informit.com/articles/article.aspx?p=23618&seqNum=14

The program in Listing 8.11 illustrates the use of setitimer to track the execution time of a program. A timer is configured to expire every 250 milliseconds and send a SIGVTALRM signal.

Modified to send SIGALRM, based on real rather than virtual time.

Listing 8.11 (itemer.c) Timer Example
*/

#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

// =======================================================
// Tunables
// #define ASM
// =======================================================

#ifdef ASM
// sytem call codes
#define SETITIMER 104
#define GETITIMER 105
#define SIGACTION 67
#endif

// in micro-sec
#define DELAY 250000

static uint64_t startT, stopT;

// adapted from wiringPi; only need for timing, not for the itimer itself
uint64_t timeInMicroseconds(){
  struct timeval tv, tNow, tLong, tEnd ;
  uint64_t now ;
  // gettimeofday (&tNow, NULL) ;
  gettimeofday (&tv, NULL) ;
  now  = (uint64_t)tv.tv_sec * (uint64_t)1000000 + (uint64_t)tv.tv_usec ; // in us
  // now  = (uint64_t)tv.tv_sec * (uint64_t)1000 + (uint64_t)(tv.tv_usec / 1000) ; // in ms

  return (uint64_t)now; // (now - epochMilli) ;
}

void timer_handler (int signum)
{
 static int count = 0;
 stopT = timeInMicroseconds();
 count++;
 fprintf(stderr, "timer expired %d times; (measured interval %f sec)\n", count, (stopT-startT)/1000000.0);
 startT = timeInMicroseconds();
}

#ifdef ASM
/* REMINDER:
int getitimer(int which, struct itimerval *curr_value);
int setitimer(int which, const struct itimerval *new_value,
              struct itimerval *old_value);
*/

static inline int getitimer_asm(int which, struct itimerval *curr_value){
  //struct itimerval *buf;
  int res;
  //buf = (struct itimerval *)malloc(sizeof(struct itimerval));
  asm(/* inline assembler version of performing a system call to GETITIMER */
      "\tB _bonzo105\n"
      "_bonzo105: NOP\n"
      "\tMOV R0, %[which]\n"
      "\tLDR R1, %[buffer]\n"
      "\tMOV R7, %[getitimer]\n"
      "\tSWI 0\n"
      "\tMOV %[result], R0\n"
      : [result] "=r" (res)
      : [buffer] "m" (curr_value)
	, [which] "r" (ITIMER_REAL)
	, [getitimer] "r" (GETITIMER)
      : "r0", "r1", "r7", "cc");
  fprintf(stderr, "ASM: getitimer has returned a value of %d \n", res);
}

static inline int setitimer_asm(int which, const struct itimerval *new_value,
				struct itimerval *old_value) {
  // struct itimerval *buf1, *buf2;
  int res;
  // buf1 = (struct itimerval *)malloc(sizeof(struct itimerval));
  // buf2 = (struct itimerval *)malloc(sizeof(struct itimerval));
  fprintf(stderr, "ASM: calling setitimer with this struct itimerval contents: %d secs %d micro-secs \n", 
	  new_value->it_interval.tv_sec, new_value->it_interval.tv_usec);
  asm(/* inline assembler version of performing a system call to SETITIMER */
      "\tB _bonzo104\n"
      "_bonzo104: NOP\n"
      "\tMOV R0, %[which]\n"
      "\tLDR R1, %[buffer1]\n"
      "\tLDR R2, %[buffer2]\n"
      "\tMOV R7, %[setitimer]\n"
      "\tSWI 0\n"
      "\tMOV %[result], R0\n"
      : [result] "=r" (res)
      : [buffer1] "m" (new_value)
        , [buffer2] "m" (old_value)
	, [which] "r" (ITIMER_REAL)
	, [setitimer] "r" (SETITIMER)
      : "r0", "r1", "r2", "r7", "cc");
  fprintf(stderr, "ASM: setitimer has returned a value of %d \n", res);

}

/* REMINDER:
int sigaction(int signum, const struct sigaction *act,
                     struct sigaction *oldact);
*/
int sigaction_asm(int signum, const struct sigaction *act, struct sigaction *oldact){
  // struct sigaction *buf;
  int res;

  asm(/* inline assembler version of performing a syscall to SIGACTION */
      "\tB _bonzo67\n"
      "_bonzo67: NOP\n"
      "\tMOV R0, %[signum]\n"
      "\tLDR R1, %[buffer1]\n"
      "\tLDR R2, %[buffer2]\n"
      "\tMOV R7, %[sigaction]\n"
      "\tSWI 0\n"
      "\tMOV %[result], R0\n"
      : [result] "=r" (res)
      : [buffer1] "m" (act)
        , [buffer2] "m" (oldact)
	, [signum] "r" (signum)
	, [sigaction] "r" (SIGACTION)
      : "r0", "r1", "r2", "r7", "cc");
  fprintf(stderr, "ASM: sigaction has returned a value of %d \n", res);

}

#endif /* ASM */

int main ()
{
 struct sigaction sa;
 struct itimerval timer;

#ifdef ASM
 fprintf(stderr, "configuring a timer with a delay of %d micro-seconds (Assembler version) ...\n", DELAY);
#else
 fprintf(stderr, "configuring a timer with a delay of %d micro-seconds (C version) ...\n", DELAY);
#endif

 /* Install timer_handler as the signal handler for SIGALRM. */
 memset (&sa, 0, sizeof (sa));
 sa.sa_handler = &timer_handler;
 // ORIG: sigaction (SIGVTALRM, &sa, NULL);
 /* From the man page:
       The  sigaction()  system  call  is used to change the action taken by a
       process on receipt of a specific signal.  (See signal(7) for  an  over‐
       view of signals.)
       signum  specifies the signal and can be any valid signal except SIGKILL
       and SIGSTOP.
       If act is non-NULL, the new action for signal signum is installed  from
       act.  If oldact is non-NULL, the previous action is saved in oldact.

 */
#if defined(ASM)
 sigaction_asm (SIGALRM, &sa, NULL);
#else
 sigaction (SIGALRM, &sa, NULL);
#endif

 /* Configure the timer to expire after 250 msec... */
 timer.it_value.tv_sec = 0;
 timer.it_value.tv_usec = DELAY;
 /* ... and every 250 msec after that. */
 timer.it_interval.tv_sec = 0;
 timer.it_interval.tv_usec = DELAY;
 /* Start a virtual timer. It counts down whenever this process is executing. */
 // ORIG: setitimer (ITIMER_VIRTUAL, &timer, NULL);
#if defined(ASM)
 setitimer_asm (ITIMER_REAL, &timer, NULL);
#else
 setitimer (ITIMER_REAL, &timer, NULL);
#endif

 /* Do busy work. */
 startT = timeInMicroseconds();
 while (1) {} ;
}

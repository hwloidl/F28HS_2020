/*
Simple example code of using an itimer (interval timer) through standard C library functions

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

int main ()
{
 struct sigaction sa;
 struct itimerval timer;

 fprintf(stderr, "configuring a timer with a delay of %d micro-seconds ...\n", DELAY);

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
 sigaction (SIGALRM, &sa, NULL);

 /* Configure the timer to expire after 250 msec... */
 timer.it_value.tv_sec = 0;
 timer.it_value.tv_usec = DELAY;
 /* ... and every 250 msec after that. */
 timer.it_interval.tv_sec = 0;
 timer.it_interval.tv_usec = DELAY;
 /* Start a virtual timer. It counts down whenever this process is executing. */
 // ORIG: setitimer (ITIMER_VIRTUAL, &timer, NULL);
 setitimer (ITIMER_REAL, &timer, NULL);

 /* Do busy work. */
 startT = timeInMicroseconds();
 while (1) {} ;
}

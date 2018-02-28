/*
  Basic example of a signal handler for CTRL-C software interrupts from the shell.

  Base version from: https://www.cs.cf.ac.uk/Dave/C/node24.html#SECTION002400000000000000000

  Description:
  A program to trap a ctrl-c but not quit on this signal. 
  We have a function sigproc() that is executed when we trap a ctrl-c. 
  We will also set another function to quit the program if it traps the SIGQUIT signal so we can terminate our program: 
*/

// import the relevant header files
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

// declare the types of signal handler functions (needed in main, be before they are defined)
void sigproc(int unused);
void quitproc(int unused); 

/* Reminder: the interface for installing a signal handler is this (see man 2 signal):
NAME
       signal - ANSI C signal handling
SYNOPSIS
       #include <signal.h>
       typedef void (*sighandler_t)(int);
       sighandler_t signal(int signum, sighandler_t handler);
DESCRIPTION
       The behavior of signal() varies across UNIX versions, and has also var‐
       ied historically across different versions of Linux.   Avoid  its  use:
       use sigaction(2) instead.  See Portability below.

       signal() sets the disposition of the signal signum to handler, which is
       either SIG_IGN, SIG_DFL, or the address of a  programmer-defined  func‐
       tion (a "signal handler").
 */

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 
void main(){		 
  signal(SIGINT, sigproc);   // install a custom signal handler for CTRL-C
  signal(SIGQUIT, quitproc); // install a custom signal handler for CTRL-\ 
  printf("ctrl-c disabled use ctrl-\\ to quit\n");
  for(;;); /* infinite loop */
}

/* signal handler; must match the type sighandler_t */
void sigproc(int unused) { 		 
  signal(SIGINT, sigproc); /*  */
  /* NOTE some versions of UNIX will reset signal to default
     after each call. So for portability reset signal each time */
  
  printf("you have pressed ctrl-c; press ctrl-\\ to quit \n");
}
 
/* signal handler; must match the type sighandler_t */
void quitproc(int unused){ 		 
  printf("ctrl-\\ pressed to quit\n");
  exit(0); /* normal exit status */
}


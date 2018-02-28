/* 
   Use Rpi 2 on-chip timer to implement a delay (using busy-wait)

   Compile: gcc -O -o itimer31 itimer31.c
   Run:     sudo ./itimer31
 */

/* --------------------------------------------------------------------------- */
/* imports */

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

#include <string.h> 
#include <signal.h>
#include <signal.h>
#include <stdint.h>

//#include <wiringPi.h> 
#include <unistd.h>

#include <sys/ioctl.h>

// #include "wiringPi.h"

#ifndef	TRUE
#define	TRUE	(1==1)
#define	FALSE	(1==2)
#endif

#define	PAGE_SIZE		(4*1024)
#define	BLOCK_SIZE		(4*1024)

#define	INPUT			 0
#define	OUTPUT			 1

#define	LOW			 0
#define	HIGH			 1

/* --------------------------------------------------------------------------- */
/* App config */

// Define the pins that we will use and the unit time. We will set up the pin 2 and have a unit time of 500 ms.

#define LED 12
#define BUTTON 16

// now all in micro-seconds
#define TIMEOUT 7000000

// OLD
// #define DELAY 700
// #define TIMEOUT 2000

/* ------------------------------------------------------- */
/* constants */

static volatile unsigned int gpiobase ;
static volatile uint32_t *gpio ;

static volatile unsigned int timerbase ;
static volatile uint32_t *timer ;

static uint64_t startT, stopT;

/* ------------------------------------------------------- */
/* timer functions */

void delay(ulong howLong)
{
  volatile uint32_t ts = *(timer+1); // word offset
  uint32_t n=0, m=0;
  uint32_t last, curr;

#if defined(DEBUG)
  fprintf (stderr, "Waiting for %d micro-seconds (reading counter value from %x)\n", howLong, (timer+1));
  last = *(timer+1); 
#endif
  while( ( (curr=*(timer+1)) - ts ) < howLong ) { /* BLANK */ }
}

/* --------------------------------------------------------------------------- */
/* aux fcts */

// adapted from wiringPi
uint64_t timeInMicroseconds(){
  struct timeval tv;
  uint64_t now ;
  gettimeofday (&tv, NULL) ;
  now  = (uint64_t)tv.tv_sec * (uint64_t)1000000 + (uint64_t)tv.tv_usec ; // in us
  // now  = (uint64_t)tv.tv_sec * (uint64_t)1000 + (uint64_t)(tv.tv_usec / 1000) ; // in ms

  return (uint64_t)now; 
}

int failure (int fatal, const char *message, ...)
{
  va_list argp ;
  char buffer [1024] ;

  if (!fatal) //  && wiringPiReturnCodes)
    return -1 ;

  va_start (argp, message) ;
  vsnprintf (buffer, 1023, message, argp) ;
  va_end (argp) ;

  fprintf (stderr, "%s", buffer) ;
  exit (EXIT_FAILURE) ;

  return 0 ;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

int main(int argc, char **argv){
  int found = 0, attempts = 0, i, j, code;
  int c, d, buttonPressed, rel, foo;
  int *attSeq;

  int pinLED = LED, pinButton = BUTTON;
  int fSel, shift, pin,  clrOff, setOff, off, res;
  int   fd ;

  int  exact, contained;
  char str1[32];
  char str2[32];
  
  struct timeval t1, t2 ;
  int t ;


  printf ("Testing program for the on-chip timer of the Raspberry Pi 2 (timeout %d us)\n", TIMEOUT) ;

  if (geteuid () != 0)
    fprintf (stderr, "PiSetup: Must be root. (Did you forget sudo?)\n") ;


  // -----------------------------------------------------------------------------
  // constants for RPi2
  // GPIO peripherals
  gpiobase = (unsigned int)0x3F200000 ;

  // timer peripherals
  timerbase = (unsigned int)0x3F003000 ;
  // -----------------------------------------------------------------------------
  // memory mapping 
  // Open the master /dev/memory device

  if ((fd = open ("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC) ) < 0)
    return failure (FALSE, "setup: Unable to open /dev/mem: %s\n", strerror (errno)) ;

  // GPIO:
  gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, gpiobase) ;
  if ((int32_t)gpio == (int32_t)MAP_FAILED)
    return failure (FALSE, "setup: mmap (GPIO) failed: %s\n", strerror (errno)) ;
  else
    fprintf(stderr, "NB: gpio = %x for gpiobase %x\n", gpio, gpiobase);

  // TIMER:
  timer = (int32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, timerbase) ;
  if ((int32_t)timer == (int32_t)MAP_FAILED)
    return failure (FALSE, "setup: mmap (TIMER) failed: %s\n", strerror (errno)) ;
  else
    fprintf(stderr, "NB: timer = %x for timerbase %x\n", timer, timerbase);

  // -----------------------------------------------------------------------------
  // main loop: do a busy wait for a given number of micro-seconds
  fprintf(stderr, "Waiting for %lu micro-seconds (%lu sec) using RPi~2 timer ...\n", TIMEOUT, TIMEOUT/1000000);
  fflush(stderr);
  startT = timeInMicroseconds();
  {
    volatile uint32_t ts = *(timer+1); // word offset
    uint32_t last, curr;

    while( ( (curr=*(timer+1)) - ts ) < TIMEOUT )  {  /* nothing, i.e. busy wait */ }
  }
  stopT = timeInMicroseconds();
  fprintf(stderr, "Done. Measured time: %lu us\n", (ulong)stopT-startT);

  fprintf(stderr, "Same again, using our own delay function: waiting for %lu micro-seconds (%lu sec) using RPi~2 timer ...\n", TIMEOUT, TIMEOUT/1000000);
  fflush(stderr);
  startT = timeInMicroseconds();
  delay(TIMEOUT);
  stopT = timeInMicroseconds();
  fprintf(stderr, "Done. Measured time: %lu us\n", (ulong)stopT-startT);

}

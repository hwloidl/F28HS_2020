// Blinking LED, now really standalone; LED controlled from C level
// Compile: gcc  -o  t1 tut_led.c
// Run:     sudo ./t1

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

// =======================================================
// Tunables
// The OK/Act LED is connected to BCM_GPIO pin 47 (RPi 2)
#define ACT  47
// delay for blinking
#define DELAY 700
// =======================================================

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


static volatile unsigned int gpiobase ;
static volatile uint32_t *gpio ;

// -----------------------------------------------------------------------------

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
/* HaWo: tinkering starts here */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

int main (void)
{
  int pinACT = ACT; // ,  pinLED = LED, pinButton = BUTTON;
  int fSel, shift, pin, clrOff, setOff;
  int   fd ;
  int   j;
  int theValue, thePin;
  unsigned int howLong = DELAY;
  uint32_t res; /* testing only */

  printf ("Raspberry Pi blinking LED %d\n", ACT) ;

  if (geteuid () != 0)
    fprintf (stderr, "setup: Must be root. (Did you forget sudo?)\n") ;

  // -----------------------------------------------------------------------------
  // constants for RPi2
  gpiobase = 0x3F200000 ;

  // -----------------------------------------------------------------------------
  // memory mapping 
  // Open the master /dev/memory device

  if ((fd = open ("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC) ) < 0)
    return failure (FALSE, "setup: Unable to open /dev/mem: %s\n", strerror (errno)) ;

  // GPIO:
  gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, gpiobase) ;
  if ((int32_t)gpio == -1)
    return failure (FALSE, "setup: mmap (GPIO) failed: %s\n", strerror (errno)) ;
  else
    fprintf(stderr, "NB: gpio = %x for gpiobase %x\n", gpio, gpiobase);

  // -----------------------------------------------------------------------------
  // setting the mode
  fprintf(stderr, "setting pin %d to %d ...\n", pinACT, OUTPUT);
  fSel =  4;    // GPIO 47 lives in register 4 (GPFSEL)
  shift =  21;  // GPIO 47 sits in slot 7 of register 4, thus shift by 7*3 (3 bits per pin)
  *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) | (1 << shift) ;  // Sets bits to one = output

  // -----------------------------------------------------------------------------

  // now, start a loop, listening to pinButton and if set pressed, set pinLED
 fprintf(stderr, "starting loop ...\n");
 for (j=0; j<1000; j++)
  {
    theValue = ((j % 2) == 0) ? HIGH : LOW;

    if ((pinACT & 0xFFFFFFC0 /* PI_GPIO_MASK */) == 0)	// bottom 64 pins belong to the Pi	
      {
	int off = (theValue == LOW) ? 11 : 8; // ACT/LED 47; register number for GPSET/GPCLR
        *(gpio + off) = 1 << (pinACT & 31) ;
      }
      else
      {
        fprintf(stderr, "only supporting on-board pins\n");          exit(1);
       }

    // INLINED delay
    {
      struct timespec sleeper, dummy ;

      // fprintf(stderr, "delaying by %d ms ...\n", howLong);
      sleeper.tv_sec  = (time_t)(howLong / 1000) ;
      sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

      nanosleep (&sleeper, &dummy) ;
    }
  }
 // Clean up: write LOW
 clrOff = 11; 
 *(gpio + clrOff) = 1 << (pinACT & 31) ;

 fprintf(stderr, "end main.\n");
}

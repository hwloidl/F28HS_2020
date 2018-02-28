// Blinking LED, now really standalone; LED controlled from C level
// Same as tinkerHaWo33.c but using different pins: pin 23 for LED

// Compile: gcc  -o t tut_led_pin23.c
// Run:     sudo ./t

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
// PINs (based on BCM numbering)
#define LED 23
#define BUTTON 24
// delay for loop iterations (mainly), in ms
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


// Mask for the bottom 64 pins which belong to the Raspberry Pi
//	The others are available for the other devices

#define	PI_GPIO_MASK	(0xFFFFFFC0)

/* ------------------------------------------------------- */

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

/* Main ----------------------------------------------------------------------------- */

int main (void)
{
  int pinLED = LED, pinButton = BUTTON;
  int fSel, shift, pin,  clrOff, setOff, off;
  int   fd ;
  int   j;
  int theValue, thePin;
  unsigned int howLong = DELAY;
  uint32_t res; /* testing only */

  printf ("Raspberry Pi button controlled LED (button in %d, led out %d)\n", BUTTON, LED) ;

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

  // -----------------------------------------------------------------------------
  // setting the mode
  // controlling LED pin 23
  fSel =  2;    // GPIO 23 lives in register 2 (GPFSEL2)
  shift =  9;  // GPIO 23 sits in slot 3 of register 2, thus shift by 3*3 (3 bits per pin)
  // C version of setting LED
  *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) | (1 << shift) ;  // Sets bits to one = output

  // -----------------------------------------------------------------------------

  // now, start a loop, listening to pinButton and if set pressed, set pinLED
 fprintf(stderr, "starting loop ...\n");
 for (j=0; j<1000; j++)
  {
    theValue = ((j % 2) == 0) ? HIGH : LOW;
    if ((pinLED & 0xFFFFFFC0 /*PI_GPIO_MASK */) == 0)		
      {
	off = (theValue == LOW) ? 10 : 7; // LED 23; register number for GPSET0/GPCLR0
	*(gpio + off) = 1 << (LED & 31) ;
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
 *(gpio + 10) = 1 << (23 & 31) ;

 fprintf(stderr, "end main.\n");
}

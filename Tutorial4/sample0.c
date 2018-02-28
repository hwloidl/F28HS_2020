/*
  From: http://www.ethernut.de/en/documents/arm-inline-asm.html
  Rotating bits example.

  Compile: gcc -o s0 sample0.c
  Run:     ./s0
 */

//  The next example of rotating bits passes C variables to assembly language. It takes the value of one integer variable, right rotates the bits by one and stores the result in a second integer variable.

#include <stdio.h>
#include <stdlib.h>

static volatile int val = 1024, val2, val3;

void main (int argc, char **argv) {
  fprintf(stderr, "Value = %d\n", val);

  /* Rotating bits example: take variable val as input, perform operation, and output into var val2 */
  asm("mov %[result], %[value], ror #1" : [result] "=r" (val2) : [value] "r" (val));

  fprintf(stderr, "Value after rotate-right by 1 of value %d (expect %d /2 = %d): %d\n", val, val, val/2, val2);

  /* Rotating bits example: take variable val as input, perform operation, and output into var val2 */
  asm("mov %[result], %[value], ror #1" : [result] "=r" (val2) : [value] "r" (val));
  asm("orr %[result], %[result], #0x7" : [result] "=r" (val2) : [value] "r" (val));

  fprintf(stderr, "Value after rotate-right by 1 followed by OR with literal 0x7 ((expect %d / 2 + 7 = %d): %d\n", val, val/2+7, val2);

  /* Rotating bits example: take variable val as input, perform operation, and output into var val2 */
  asm(/* multi-line example */
      "\tMOV R0, %[value]\n"         /* load the address into R0 */ 
      "\tLDR %[result], [R0, #0]\n"  /* get and return the value at that address */
      : [result] "=r" (val3) 
      : [value] "r" (&val)
      : "r0", "cc" );

  fprintf(stderr, "Value lookup at address %x (expect %d): %d\n", &val, val, val3);
}

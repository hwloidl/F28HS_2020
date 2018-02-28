/*
  Based on: http://www.ethernut.de/en/documents/arm-inline-asm.html

  min-max example: take a pair as input and move smaller value to min, larger value to max field.
  C and inline assembler versions

  Compile: gcc -o s01 sample01.c
  Run:     ./s01
 */

#include <stdio.h>
#include <stdlib.h>

static volatile int val = 1024, val2, val3;
typedef struct {
  ulong min;
  ulong max;
} pair_t;

// pair_t pair1, res1, res2, res3, res4, res5, res_ex;

void main (int argc, char **argv) {
   pair_t *pair = (pair_t*)malloc(sizeof(pair_t));
   pair_t *pair2 = (pair_t*)malloc(sizeof(pair_t));

   pair->min = 7;
   pair->max = 5;
   
  fprintf(stderr, "Testing simple inline assembler programs\nValue = %d\n", val);
  {
   ulong t;
   fprintf(stderr, "BEFORE: pair min: %d\tpair max: %d\n", pair->min, pair->max);
   if (pair->min > pair->max) {
     t = pair->min;
     pair->min = pair->max;
     pair->max = t;
   }
   fprintf(stderr, "AFTER:  pair min: %d\tpair max: %d\n", pair->min, pair->max);
  }  

  pair->min = 7;
  pair->max = 5;

  {
    pair_t *res;
    fprintf(stderr, "BEFORE: pair min: %d\tpair max: %d\n", pair->min, pair->max);
   
    asm volatile(/* min-max example: move smaller value to min, larger value to max field
    		    Input:  pair (C variable, pointing to a pair)
                    Output: res  (C variable, pointing to a pair)
    	       */
    	       "\tLDR R0, [%[inp], #0]  @ load first element of the pair into R0 \n"  
    	       "\tLDR R1, [%[inp], #4]  @ load second element of the pair into R1 (why the #4?)\n"
    	       "\tCMP R0, R1            @ compare the values\n"
    	       "\tBLE done%=            @ min < max means: nothing to do\n"
    	       "\tMOV R3, R0            @ otw swap the values using R3 as temp reg\n"
    	       "\tMOV R0, R1\n"
    	       "\tMOV R1, R3\n"
    	       "done%=: STR R0, [%[inp], #0] @ now store the first value in min field\n"
    	       "\tSTR R1, [%[inp], #4]      @ now store the second value in max field\n"
    	       "\tMOV %[result], %[inp]     @ finally, return the pointer to the pair\n"
                 : [result] "=r" (res)      // the name 'result' in Asm is the same as the C var 'res', and it must be held in a register ("r")
    	       : [inp] "r" (pair)           // the name 'inp' in Asm is the same as the C var 'pair', and it must be held in a register ("r")
                 : "r0", "r1", "r3", "cc"  ); // these register are modified by the above assembler code
     fprintf(stderr, "AFTER:  pair min: %d\tpair max: %d\n", pair->min, pair->max);
     fprintf(stderr, "AFTER:  res min: %d\tres max: %d\n", res->min, res->max);
     fprintf(stderr, "Note: the C variable 'res' (0x%x) is now an alias to the input variable 'pair' (0x%x); \nno space has been allocated for res, is this a bug in the program?\n", res, pair);
     pair->min = 1;
     fprintf(stderr, "After setting pair->min = 1 we have: pair min: %d\tpair max: %d\n", pair->min, pair->max);
     fprintf(stderr, "BUT also: res min: %d\tres max: %d\n", res->min, res->max);
  }
  
  *pair2 = *pair;
  fprintf(stderr, "We have copied the contents of pair into pair2: pair2 min: %d\tpair2 max: %d\n", pair2->min, pair2-> max);
  fprintf(stderr, "BUT these occupy different locations in memory: pair (0x%x) and pair2 (0x%x)\n", pair, pair2);

  { pair_t *pair3;
     fprintf(stderr, "What happens if we hadn't malloc'ed space for pair2?\n");
     *pair3 = *pair; // expect a crash!
   
  }

}

/*
  Sum example, with a smaller swap example to introduce inline assembler
  Compile:  gcc -UDEBUG -o sumav1_asm sumav1_asm.c
  Run:      ./sumav1_asm 8000
*/

#include <stdio.h>
#include <stdlib.h>

typedef unsigned long int   ulong;

// Note: the following is already defined in sys/types.h
// typedef unsigned long int ulong;

// Note: the following is already defined in stdint.h
// typedef unsigned int            uint32_t

typedef struct {
  ulong min;
  ulong max;
} pair_t;

pair_t res1, res2, res3, res4, res5, res_ex;

void minmax_c(pair_t *pair) {
  ulong t;
  if (pair->min > pair->max) {
    t = pair->min;
    pair->min = pair->max;
    pair->max = t;
  }
}  

ulong sumpair_asm(pair_t *pair) {
  ulong res;
  asm volatile(/* sum over int values */
	       "\tLDR R0, [%[inp], #0]\n"
	       "\tLDR R1, [%[inp], #4]\n"
	       "\tADD R0, R0, R1\n"
	       "\tMOV %[result], R0\n"
               : [result] "=r" (res)
	       : [inp] "r" (pair)
               : "r0", "r1", "cc"  );
  return res;
}

    
void minmax_asm(pair_t *pair) {
  pair_t *res;
  asm volatile(/* sum over int values */
	       "\tLDR R0, [%[inp], #0]\n"
	       "\tLDR R1, [%[inp], #4]\n"
	       "\tCMP R0, R1\n"
	       "\tBLE done\n"
	       "\tMOV R3, R0\n"
	       "\tMOV R0, R1\n"
	       "\tMOV R1, R3\n"
	       "done: STR R0, [%[inp], #0]\n"
	       "\tSTR R1, [%[inp], #4]\n"
               : [result] "=r" (res)
	       : [inp] "r" (pair)
               : "r0", "r1", "r3", "cc"  );
}


#ifdef SUM
/* Initialise an array with the numbers from 1 to n */
void initArr(ulong *arr, int len) {
  int i, j;
  ulong x;
  if (arr==NULL)
    arr = (ulong*)malloc(len*sizeof(ulong));
  for (i=0; i<len; i++) 
    arr[i] = i+1;
}

ulong sum_c(ulong *arr, int len) {
  int i=0;
  ulong sum=0;
  fprintf(stderr, "Running C version on array at %p of length %d\n", arr, len);
  for (i=0; i<len; i++) {
    sum += arr[i];
  }
  return sum;
}

ulong sum_asm(ulong *arr, int len) {
  ulong res;
  fprintf(stderr, "Running assembler version on array at %p of length %d\n", arr, len);
  asm volatile(/* sum over int values */
               "\tMOV R2, %[a]\n" 
	       "\tMOV R3, #0\n"
	       "\tMOV R0, #0\n"
	       "loop: NOP\n"
	       "\tLDR R1, [R2, #0]\n"
	       "\tADD R0, R0, R1\n"
	       "\tADD R2, R2, %[sz]\n"
	       "\tADD R3, R3, #1\n"
	       "\tCMP R3, %[len]\n"
	       "\tBLT loop\n"
	       "\tMOV %[result], R0\n"
               : [result] "=r" (res)
	       : [a] "r" (arr)
		 , [len] "r" (len)
		 , [sz] "r" (sizeof(ulong))
               : "r0", "r1", "r2", "r3", "cc"  );
  return res;
}
#endif

void main (int argc, char **argv) {
  int len = 0;
  ulong res, t;
  ulong *arr;
  struct timeval t1, t2 ;

#ifdef SUM
  if (argc<2) {
    fprintf(stderr, "Usage: ./sample2 <len> ... sum over <len> int values\n");
    exit(1);
  }
  len = atoi(argv[1]);
  fprintf(stderr, "Length = %d\n", len);
  arr = (ulong*)malloc(len*sizeof(ulong));
  initArr(arr, len);
#endif

  { 
    pair_t pair = { 7L, 5L } ;
    fprintf(stderr, "Testing pair_minmax; input pair: %lu %lu\n", pair.min, pair.max);
    minmax_c(&pair);
    fprintf(stderr, "After C-version of minmax  pair: %lu %lu\n", pair.min, pair.max);
  }

  { 
    pair_t pair = { 7L, 5L } ;
    fprintf(stderr, "Testing pair_minmax; input pair: %lu %lu\n", pair.min, pair.max);
    minmax_asm(&pair);
    fprintf(stderr, "After Asm-version of minmax  pair: %lu %lu\n", pair.min, pair.max);
  }
  { 
    pair_t pair = { 7L, 5L } ;
    ulong res;
    res = sumpair_asm(&pair);
    fprintf(stderr, "Testing sumpair_; input pair: %lu %lu; sumpair: %d (expected %d)\n", pair.min, pair.max, res, pair.min+pair.max);
  }

#ifdef SUM
  gettimeofday (&t1, NULL) ;
  res = sum_c(arr, len);
  gettimeofday (&t2, NULL) ;
  // d = difftime(t1,t2);
  if (t2.tv_usec < t1.tv_usec)	// Counter wrapped
    t = (1000000 + t2.tv_usec) - t1.tv_usec;
  else
    t = t2.tv_usec - t1.tv_usec ;

  fprintf(stderr, "C version: Sum over %d integer numbers = %d (expected %d) (elapsed time: %d ms)\n", len, res, ((len*(len+1))/2), t);

  gettimeofday (&t1, NULL) ;
  res = sum_asm(arr, len);
  gettimeofday (&t2, NULL) ;
  // d = difftime(t1,t2);
  if (t2.tv_usec < t1.tv_usec)	// Counter wrapped
    t = (1000000 + t2.tv_usec) - t1.tv_usec;
  else
    t = t2.tv_usec - t1.tv_usec ;

  fprintf(stderr, "Assembler version: Sum over %d integer numbers = %d (expected %d) (elapsed time: %d ms)\n", len, res, ((len*(len+1))/2), t);
#endif
}


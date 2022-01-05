#include "harness.h"

// This seems to become SAFE. Instead we should unroll the typedef where
// it is used, thus allowing the pointer to become bounded
typedef int * intptr;


TESTDEF default : success


int main() {
  int array[8];
  int i;
  
IFTEST test1 : success
  intptr p;
ELSE 
  int * p;
ENDIF 
  
  for(p=array; p<array + 8; p++) {
    *p = 0;
  }

IFTEST test2 : success 
  p = (intptr)malloc(5 * sizeof(int));
ELSE
  p = (int *)malloc(5 * sizeof(int));
ENDIF

  p ++; p ++;
 
 return 0;
}

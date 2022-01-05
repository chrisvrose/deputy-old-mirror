#include "harness.h"

typedef struct {
  struct    {
    int * subp;
  } stuff;
} cell, * cellptr;


int i = 345;
cell c = { { &i } };


int main() {
IFTEST safe : success 
  {
    cellptr q = &c;
    int * SAFE p = TC(q->stuff.subp);
    
    if(*p != 345) E(1);
  }
ENDIF
  
IFTEST array : success
  {
    int array[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    // Are the bounds properly propagated ?
    int * SEQ p = TC(&array[5]);

    if(*(p - 5) != 0) E(2);

    if(*(p + 4) != 9) E(3);

  }
ENDIF 
  return 0;
}

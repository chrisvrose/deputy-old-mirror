// NUMERRORS 2

#include "harness.h"

// This is potential bug regarding overflow in multiplication.  We wrongly
// assume that k times x is always a  multiple of k.
//
// In this example, we dereference p[x] where the elements in p are 12 bytes
// long, so p[x] compiles to mem[p + 12x].  But if we take
// x = 357,913,942, then (12x mod 2^32) is 8.  Address (p + 8) is
// within bounds, yet we read the i2 field when we think we are reading
// the ptr field.

int globalint = 29;

#define NELEM 4


struct twelve{
  int* SAFE ptr;
  unsigned int i1;
  unsigned int i2;
};

int foo(unsigned int x, struct twelve* COUNT(NELEM) p) {
  int* ptr = p[x].ptr;
  printf("p = 0x%x, &p[x] = 0x%x\n", (unsigned long) p, (unsigned long) &p[x]);
  printf("Pointer is 0x%x\n", (unsigned long) ptr);
  //ptr is now 0xdeadbeef.
  return *ptr;
}


int main() {
  if (sizeof (struct twelve) != 12) E(1);

  struct twelve* p = malloc(NELEM * sizeof(struct twelve));
  for (int i = 0; i < NELEM; i++) {
    p[i].ptr = &globalint;
    p[i].i1 = (unsigned int)0xbadf00d;
    p[i].i2 = (unsigned int)0xdeadbeef;
  }
  foo(3,p);               //KEEP baseline: success
  foo(357913942,p);       //KEEP overflow: error = Assertion failed
  SUCCESS;
}

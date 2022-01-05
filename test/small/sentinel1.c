#include "harness.h"

int foo(int * SNT * p) {
  int x = 0;
  int * SNT * local = p;
  x = **local;        //KEEP deref1: error = sentinel
  
  // it's okay to cast ints to sentinel pointers.
  *local = 0xbadf00d;
  *p += (int)*p;

  //SNT implies COUNT(0).  Make sure no one refers to
  //__attribute__((sentinel)) directly, which would let them 
  //dereference a sentinel pointer.
  int * __attribute__((sentinel)) SAFE bad = *p; x = *bad; //KEEP safe: error = dereference of a sentinel

  return 0;
}

int main() {
  int buffer[256];
  int* SNT before = (int * SNT)buffer - 1;
  int* SNT after = &buffer[200];
  after += 56;  //after = buffer+256
  unsigned long bufferp = &buffer[0];

  int* x = after;  *x = 2;   //KEEP cast: error = may not be cast
  *after = 3;                //KEEP deref2: error = sentinel
  
  if (before != (bufferp - sizeof(int))) E(1);
  if (after != (bufferp + 256*sizeof(int))) E(2);

  foo(&before);

  return 0;
}
 

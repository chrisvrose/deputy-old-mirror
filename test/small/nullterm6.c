// Test that we are properly initializing local variables.

#include "harness.h"

TESTDEF baseline:success

void scribble() {
  int a[2048];
  int i;

  // Write a bunch of garbage on the stack.
  for (i = 0; i < 2048; i++) {
    a[i] = 42;
  }
}

void test() {
  int i;
  
  int * p1;
  int (NT p2)[8];
  struct foo {
    int * f;
  } p3;

  struct foo p4[8];

  struct fooNT {
    int (NT a)[8];
  } p5;

  union fooNTu {
    int * ptr WHEN(1);
    int   in;
  } p6;

  // Check that we actually scribbled on the stack.  A failure here does
  // not indicate a failure in Deputy; I'm just trying to ensure that
  // the test tests what it's supposed to be testing.  If this test starts
  // to fail, we can just remove it.
  if(i != 42) E(10);
  
  //We report this one as an error:
  if(p1 != 0) E(1);  //KEEP uninit_ptr: error = p1 may be used without being defined

  if(p2[7] != 0) E(2);

  if(p3.f != 0) E(3);

  for(i=0;i<8;i++) {
    if(p4[i].f != 0) E(4);
  }

  if(p5.a[7] != 0) E(5);

  if(p6.ptr != 0) E(6);

  SUCCESS;
}

int main() {
  scribble();
  test();

  SUCCESS;
}

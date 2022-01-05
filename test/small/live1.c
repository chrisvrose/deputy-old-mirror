#include "harness.h"

void exit(int code) __attribute__((noreturn));

int global;

TESTDEF baseline: success

int foo(int * SAFE x) {
  int *y;

  if (*x) { 
    return 0;  //DROP ret: error = y may be used without being
  } else {
    y=x;
  }
  x = y;
  return *y;
}

int bar() {
  int *x;
  int z;
  switch(global) {
  case 0:
  case 1:
    x = & global;
    break;
  case 2:
    x = malloc(*x);//KEEP arg: error = x may be used without being
    //FALLTHROUGH
  case 3:
    z = *x; //KEEP deref: error = x may be used without being
    //FALLTHROUGH
  case 4:
    //Sizeof is okay:
    x = malloc(sizeof(*x));
    break;

  default: exit(0); //DROP default: error = x may be used without being
  }
  return foo(x);
}

int main() {
  return bar();
}

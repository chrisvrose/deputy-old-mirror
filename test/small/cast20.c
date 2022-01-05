#include "harness.h"


int foo(unsigned long px)  {
  * (int * SAFE)(void * TRUSTED)px += 1;
}

int main() {
  int x = 2;

  foo(& x);

  if(x != 3) E(1);

  return 0;
}


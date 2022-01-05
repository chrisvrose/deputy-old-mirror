#include "harness.h"

typedef int * intptr;

void * (DALLOC(sz) allocate)(int sz) {
  return malloc(sz);
}

int main() {
  intptr p = (intptr) allocate(4 * sizeof(int));
  return 0;
}

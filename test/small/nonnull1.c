#include "harness.h"

//KEEP baseline: success

int global[5];

int* NONNULL COUNT(3) foo(int* COUNT(4) NONNULL x) {
  *x = 1;
  int* p = x + 1;
  *p = 2;
  return p;
}

struct test {
  int* NONNULL y;
} gstruct = { & global[2] };


int main() {
  memset(&gstruct, 0, sizeof(gstruct)); // KEEP memset: error = memset on a type containing a nonnull pointer
  struct test * SAFE p = malloc(1*sizeof(struct test)); //KEEP malloc: error = Allocation of a buffer containing non-null

  int* x = malloc(5*sizeof(*x));
  //we need a null check here; it's the only one we need in the file.
  memset(x, 0x55, 5*sizeof(*x));
  if (global[2]) {
    x = &global;
    x++;
  }
  x = foo(x);
  return *x-2;
}

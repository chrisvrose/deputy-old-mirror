#include "harness.h"

//On the second time around the loop, we get an assertion failure
// when assigning size=11, because ptr depends on size.  It's unintuitive 
// to have a failure on ptr before it's declared!
// I'd like to have at least better error message for this, but I don't know
// how to do that.

// This was fixed by only considering the context to be only live
// locals, not all locals.

//KEEP baseline: success

int main() {
  for (int i = 10; i < 15; i++) {
    int size = i;
    int * COUNT(size) ptr = malloc(size*sizeof(int));
    ptr[size-1] = i;

    //This workaround ought to fix the problem, but it doesn't because 
    // CIL introduces a temp var for ptr.  We'd need to set that var
    // to zero too, but we can't because it can't be named.
    ptr = 0;   //KEEP workaround: success
  }
  return 0;
}

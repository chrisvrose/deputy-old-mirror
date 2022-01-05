
#include "harness.h"

// Test dependencies of return on the function arguments
TESTDEF baseline : success

int * COUNT(n) gettable(int n, int * SAFE pn) {
  int * res;

  (*pn)--;

  // Make sure that we use the original value of n for the return
  n --; // KEEP : error = upper bound coercion
  
  res = (int *)malloc(n * sizeof(int));

  res ++; // KEEP : error = upper bound coercion

  return res;
}


int main() {
  int n;
  int * SAFE pn = &n;

  int *
    COUNT(8) // KEEP : success
    p;
  
  p = gettable(8, pn);

  // Try to access the memory
  p[0] = 0;
  p[7] = 7;

  // Access past the array
  p[8] = 8; // KEEP : error = will always fail

  // Now make sure that we use the value of the argument when the function
  // is called (not when it returns) in the bounds.
  *pn = 8;
  p = gettable(*pn, pn);

  p[0] = 0;
  p[7] = 7;

  p[8] = 8; // KEEP : error = pointer access check
  
  return 0;
}

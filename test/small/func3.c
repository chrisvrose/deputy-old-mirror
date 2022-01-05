
#include "harness.h"


int carray; // Count of elements
int * COUNT(8) array; 

TESTDEF baseline : success

int side(int incr) {
  carray += incr;
  return carray;
}

// Allow a function to refer to global variables
void access_array(int * BND(__this - off, __this - off + carray) parray,
            int off, int extra) {
  if(parray[-off] != 0) E(1);

  // Try to decrease the global carray.
  side(-5); // KEEP side1 : success
  
  // We should be allowed to access 8 elements
  if(parray[7-off] != 8) E(2);

  // Try to increase carray
  side(2);if(parray[-off+carray] != carray) E(2); // KEEP side2 : error = Blah
  
  
}

int main() {
  int i;
  
  carray = 8;
  array = (int*)malloc(carray * sizeof(*array));
  for(i=0;i<carray;i++) {
    array[i] = i;
  }

  access_array(array + 3, 3, 0);

  // This should be Ok too
  carray --; access_array(array + 4, 3, 0); // KEEP : success

  // Make sure we do not make the check too early
  access_array(array + 4, 3, side(10));
  
  SUCCESS;
}

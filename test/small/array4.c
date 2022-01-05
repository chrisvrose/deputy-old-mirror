#include "harness.h"

#define E(n) { printf("Error %d\n", n); exit(n); }

int array[8] = { 0, 1, 2, 3, 4, 5, 6, 7}; // A global array


// A pointer in the array
int * BND(array, array + 8) parray; // KEEP : success
int * BND(array, & array[8]) parray; // KEEP : success

// A couple of errors
int * BND(& 1, 0) parray; // KEEP : error = Address-of used on a non-lvalue
int * BND(array[0][0], 0) parray; // KEEP : error = Index used on a non-array


int main() {
  parray = & array[5];

  // Access the start and the end of the array
  if(parray[-5] != 0) E(1);
  if(parray[2] != 7) E(2);

  parray ++;

  if(* parray != 6) E(3);
  
  return 0;
}

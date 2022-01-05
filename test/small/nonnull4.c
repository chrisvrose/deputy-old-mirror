
#include "harness.h"

int x = 5;
int * NONNULL a[2] = { &x, &x };

int main() {

  if(* a[0] != 5) E(1);

  // Deputy will allow us to zero the contents of a
  a[0] = 0;

  // And will not have a null check now
  * a[0] = 0; // KEEP : error = non-null

  return 0;
}


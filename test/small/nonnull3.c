#include "harness.h"

int x;
int * NONNULL g = &x;

int func(int * NONNULL nz) {
  return 0;
}

int * NONNULL * pg = & g;

int main() {
  int * z = 0;

  int * p = (int * NONNULL)z; return p; // KEEP : error = non-null check

  g = z; // KEEP : error = non-null check

  func(z); // KEEP : error = non-null check

  *pg = z; // KEEP : error = non-null check 
  
  return 0;
}

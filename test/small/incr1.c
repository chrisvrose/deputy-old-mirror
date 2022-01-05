#include "harness.h"

void add(float * SNT end,
         float * BND(__this, end) NONNULL result_block,
         float what) {

  // This will create a NONNULL local initialized to 0 !!! 
  result_block += 2;

  *result_block = what;
  
}

int main() {
  float a[4];

  add(&a[4], &a[1], 5.0);

  if(a[3] != 5.0) E(1);

  return 0;
    
}

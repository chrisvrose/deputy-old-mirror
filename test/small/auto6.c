
#include "harness.h"

#define FSEQ BND(__this, __auto)
#define SEQ BND(__auto, __auto)

// Test initializers for auto 
int a[] = { 0, 1, 2, 3};
  
int * FSEQ gf =  & a[2];

struct fat {
  int * SEQ g;
  int * SEQ ga[2];
} gstruct = { & a[2], & a[1], & a[2] };
  
  
int main() {
  
  if(gf[0] != 2) E(1);
  if(gf[1] != 3) E(2);
  gf[-1] = 0; // KEEP low1 : error = will always fail
  gf[2] = 0;  // KEEP high1 : error = Assertion failed

  
  if(gstruct.g[-2] != 0) E(11);
  if(gstruct.g[1] != 3) E(12);
  gstruct.g[-3] = 0; // KEEP low2 : error = lower bound
  gstruct.g[2] = 0;  // KEEP high2 : error = Assertion failed


  if(gstruct.ga[0][-1] != 0) E(21);
  if(gstruct.ga[0][2] != 3) E(22);
  gstruct.ga[0][-2] = 0; // KEEP low3 : error = lower bound
  gstruct.ga[0][3] = 0;  // KEEP high3 : error = Assertion failed


  if(gstruct.ga[1][-2] != 0) E(31);
  if(gstruct.ga[1][1] != 3) E(32);
  gstruct.ga[1][-3] = 0; // KEEP low4 : error = lower bound
  gstruct.ga[1][2] = 0;  // KEEP high4 : error = Assertion failed


  return 0;
}


#include "harness.h"

#define FSEQ BND(__this, __auto)
#define SEQ BND(__auto, __auto)

int a[] = { 0, 1, 2, 3};

int * SEQ g;

int * FSEQ gf;

int main() {
  int * FSEQ pf;
  // Test assignment split to split
  pf = & a[2];

  if(pf[0] != 2) E(1);
  if(pf[1] != 3) E(2);
  pf[-1] = 0; // KEEP low1 : error = will always fail
  pf[2] = 0;  // KEEP high1 : error = access check

  
  // Test assignment split to fat
  g = &a[2];

  if(g[-2] != 0) E(11);
  if(g[1] != 3) E(12);
  g[-3] = 0; // KEEP low2 : error = lower bound
  g[2] = 0;  // KEEP high2 : error = Assertion failed

  // Test assignment fat to split, from SEQ to FSEQ
  pf = g;

  if(pf[0] != 2) E(21);
  if(pf[1] != 3) E(22);
  pf[-1] = 0; // KEEP low3 : error = will always fail
  pf[2] = 0;  // KEEP high3 : error = Assertion failed


  // Test assignment split to fat, from FSEQ to SEQ
  g = pf;

  if(g[0] != 2) E(31);
  if(g[1] != 3) E(32);
  g[-1] = 0; // KEEP low4 : error = lower bound
  g[2] = 0;  // KEEP high4 : error = Assertion failed

  // Test assignment fat to fat, from SEQ to FSEQ
  gf = g;

  if(gf[0] != 2) E(41);
  if(gf[1] != 3) E(42);
  gf[-1] = 0; // KEEP low5 : error = will always fail
  gf[2] = 0;  // KEEP high5 : error = Assertion failed


  // Test assignment fat to fat, from FSEQ to SEQ
  g = gf;

  if(g[0] != 2) E(51);
  if(g[1] != 3) E(52);
  g[-1] = 0; // KEEP low6 : error = lower bound
  g[2] = 0;  // KEEP high6 : error = Assertion failed

  
  return 0;
}


#include "harness.h"

#define FSEQ BND(__this, __auto)
#define SEQ BND(__auto, __auto)

TESTDEF baseline : success

int a[] = { 0, 1, 2, 3};

int * SEQ g;

struct fat {
  int * SEQ s;
  int * FSEQ f;
  int * SEQ as[4];
} gstruct;

// A function that returns a fat
int * SEQ ret_fat() {
  return &a[2];
}

// A function that returns a split
int * BND(__this, __this + 2) ret_split() {
  return &a[2];
}

int main() {

  int * p;

  // Return from fat to split
  p = ret_fat();
 
  if(p[-2] != 0) E(1);
  if(p[1] != 3) E(2);
  p[-3] = 0; // KEEP low1 : error = lower bound
  p[2] = 0;  // KEEP high1 : error = Assertion failed

  // Return from fat to fat
  g = ret_fat();
 
  if(g[-2] != 0) E(11);
  if(g[1] != 3) E(12);
  g[-3] = 0; // KEEP low2 : error = lower bound
  g[2] = 0;  // KEEP high2 : error = Assertion failed

  // Return from split to fat
  p = ret_split();
 
  if(p[0] != 2) E(21);
  if(p[1] != 3) E(22);
  p[-1] = 0; // KEEP low3 : error = will always fail
  p[2] = 0;  // KEEP high3 : error = will always fail

  // Return from split to fat
  g = ret_split();
 
  if(g[0] != 2) E(31);
  if(g[1] != 3) E(32);
  g[-1] = 0; // KEEP low4 : error = lower bound
  g[2] = 0;  // KEEP high4 : error = Assertion failed
 
  return 0;
}

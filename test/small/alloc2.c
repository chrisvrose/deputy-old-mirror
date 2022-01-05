//This test should work without any changes.
//KEEP baseline: success

#include "harness.h"

int main() {
  int len = 260;
  int * COUNT(len) p;
  p = alloc(int, len);
  p = alloc(int, len-1); //KEEP wronglen: error
  p = alloc(char, len);  //KEEP wrongtype: error = Assertion failed
  p = alloc(int*, len);

  //Don't strip the cast to char!  We shouldn't think we're allocating
  //260 ints when we really only allocate four.
  p = alloc(int, (char)len);  //KEEP cast: error = Assertion failed

  p[12] = 12;
  return 0;
}

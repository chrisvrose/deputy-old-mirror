//This test should work without any changes.
//KEEP baseline: success

#include "harness.h"

int * COUNT(42) myalloc() {
  int len = 50;
  int * COUNT(len) p;
  len = 41;            //KEEP wronglen: error
  p = alloc(int, len);
  p[12] = 12;
  return p;
}

int main() {
  int * COUNT(42) p = myalloc();
  return 0;
}

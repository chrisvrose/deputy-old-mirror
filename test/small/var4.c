// KEEP baseline: success

#include "harness.h"

int ten() {
  return 10;
}

int main() {
  int len = 5;
  int * COUNT(len) p = 0;
  //p is 0, so we can change len here
  len = 8;     //KEEP dep1: success

  p = alloc(int, len);
  p++;         //KEEP incr: error
  //Can't increase the length here:
  len = 10;    //KEEP dep2: error
  // or here:
  len = ten(); //KEEP dep3: error
  //but decreasing is okay:
  len--;
  return *p; //use p so that it's live
}

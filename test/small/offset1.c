//KEEP baseline: success
#include "harness.h"

typedef struct {
  char foo[32];
  int scalar;
} message_t;

int x = 0;

//
// Make sure deputy supports the standard trick for offsetof
//


int main() {
  if (32 != & ((message_t *)0)->scalar) E(1);
  //array case: & is changed to StartOf
  if (0 != (unsigned )& ((message_t *)0)->foo) E(2);


  //This is safe, but there's no reason to support it:
  if (33 != & ((message_t *)1)->scalar) E(3); //KEEP 1: error = Type mismatch

  x = & ((message_t *)&x)->scalar; //KEEP 2: error = will always fail

  return 0;
}

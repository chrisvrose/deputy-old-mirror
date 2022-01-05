#include <string.h>
#include "harness.h"

//Test strlcpy.  This test is not in the regression suite, because
//strlcpy doesn't work on Manju.


char* NTS global = "Really long string.  etc., etc., etc.";

TESTDEF baseline: success

int main() {
  char buf[10];
  
  //The buffer size is 10, so we should pass 9 to strncpy and 10 to strlcpy

  strncpy(buf, global, 9);
  strncpy(buf, global, 10); //KEEP strncpy: error = Assertion failed

  strlcpy(buf, global, 10);
  if (buf[9] != 0) E(3);

  //make sure strlcat works too
  buf[5] = 0;
  strlcat(buf, global, 10);
  if (buf[9] != 0) E(4);

  SUCCESS;
}

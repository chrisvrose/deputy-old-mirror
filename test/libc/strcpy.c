#include <string.h>
#include "harness.h"

char* NTS global = "Foo";

TESTDEF baseline: success

void foo(char* NT COUNT(len) buf, int len) {
  //unsafe API.  Should fail statically
  strcpy(buf, foo); //KEEP strcpy: success = Calls to strcpy are unsafe

  //we change this to strncpy(buf, "Hi, there", 9), based on the length of
  // the string literal.
  strcpy(buf, "Hi, there");
  strcpy(buf, "Hi, there."); //KEEP strcpy_toolong: error = Assertion failed


  strcpy(buf, "Hi");
  if (strcmp(buf, "Hi") != 0) E(2)
}

int main() {
  char stringbuf[10];
  foo(stringbuf, sizeof(stringbuf)-1);
  SUCCESS;
}

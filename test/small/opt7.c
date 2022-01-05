#include "harness.h"
//baseline fails when compiled with -fwritable-strings and --deputyopt=3
//KEEP baseline: success

//Don't do forward substitution with string literals.  If -fwritable-strings is
//used, each appearance of a string literal gets its own pointer.
//
//Even if -fwritable-strings is not used, is it okay to assume that string
//literals will be coalesced?

int main() {
  char * p = NTDROP("Hello.");
  int sum = 0;
  while(*p != '.') {
    sum += *p;
    p++;
    p += 8; //KEEP ubound: error = upper
  }
  int desiredsum = 'H'+'e'+'l'+'l'+'o';
  if (sum != desiredsum) E(3);
  return 0;
}

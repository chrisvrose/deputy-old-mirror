
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define E(n) { printf("Error %d\n", n); exit(n); }

int main() {
  char * str = "aabbcccdef";

  if(strspn(str, "abc") != 7) E(1);

  if(strcspn(str, "cdef") != 4) E(2);

  return 0;
}

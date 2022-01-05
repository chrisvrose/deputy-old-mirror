#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define E(n) { printf("Error %d\n", n); exit(n); }

char * NT str = "a string";

TESTDEF : success

int main() {
  int len = strlen(str);
  char * dest;
  // Forget about the last character
  len --; // KEEP : error = upper bound
  dest = (char*)malloc(len + 1); // Allocate a buffer for string + NULL
  strncpy(dest, str, strlen(str));

  if(strcmp(dest, str)) E(1);

  // Make sure that we still have 0 at the end of dest
  if(dest[len] != 0) E(2);

  return 0;
}

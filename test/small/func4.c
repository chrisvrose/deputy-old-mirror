
TESTDEF baseline : success

TESTDEF noproto : error = no prototype

#include "harness.h" // DROP noproto

int (*fptr)();

// Calling functions without prototype not allowed
// At least in the situations when pointers are involved
int main() {
  char * a;

  a = malloc(1); 

  return 0;
}

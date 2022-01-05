#include <string.h>

int main() {
  char (NT buffer1)[16];
  char buffer2[16]; // We do not need NT annotations on local buffers
  char * NTS res;
  
  res = strncpy(buffer1, "a string", sizeof(buffer1) - 1); // KEEP t1: success

  // The size argument must not include the null terminator.
  strncpy(buffer1, "a string", sizeof(buffer1)); // KEEP e1: error = upper bound

  strncpy(buffer2, "a string", 1); // KEEP t2: success

  return 0;
}

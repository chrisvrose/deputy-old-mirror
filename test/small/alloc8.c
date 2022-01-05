#include "harness.h"

TESTDEF baseline : success

char * NTS NONNULL
cpy(char * NT NONNULL COUNT(sz) buff, char * NTS str, int sz) {
  
  int i;
  for(i=0;i<sz;i++) {
    buff[i] = str[i];
  }
  buff[sz] = '\0';
  return buff;
}

int main() {
  // Problem if we call malloc as the argumnent.
  char * t = cpy(malloc(6), "abcde", 5);
  if(t[0] != 'a') E(1);
  if(t[4] != 'e') E(2);
  if(t[5] != '\0') E(3);

  if(t[6] == 0) E(4); // KEEP : error = upper bound
  
  return 0;
}

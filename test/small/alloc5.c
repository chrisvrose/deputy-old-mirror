#include "harness.h"

//When allocating an NT buffer, subtract one from the length.

//KEEP baseline: success

void my_strlcpy(char* COUNT(n-1) NT dest, char* NTS src, int n) {
  int position;
  for (position = 0; position < n-1 && *src != 0; src++, position++) {
      dest[position] = *src;
  }
  if (position < n) {
      *dest = 0;
  }
}

int main() {
    int len = 5;
    char * NT buf = alloc(char, len);
    buf[0] = buf[1] = buf[2] = buf[3] = 'a';
    buf[4] = 0;
    buf[4] = 'b';   //KEEP write: error = nullterm write check

    my_strlcpy(buf, "Hola", 5);
    my_strlcpy(buf, "Hello", 6); //KEEP call: error = nullterm upper bound
    return 0;
}

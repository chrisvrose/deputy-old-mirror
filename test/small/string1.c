// KEEP baseline: success

#include "harness.h"

int my_strlen(char * NTS str) {
  return 5;
}

void my_memcpy(char * NT COUNT(len) dest,
               char * NT COUNT(len) src,
               int len) {
  int i;
  for (i = 0; i < len; i++) {
    dest[i] = src[i];
  }
  dest[len] = src[len];  //copy the NULL
}

void foo(char * NTS str) {
  char c = *str;
  *str = 0;
}

void addr (char chr) {
  char* NTS str = "";
  char c = *str;  //legal
  //Don't create a COUNT(1) pointer to *str, because that would let us 
  //overwrite the null.
  char* NTS str2 = &(*str);  //KEEP addrofnull: error = Assertion
  *str2 = chr;              //KEEP addrofnull
}

void cast(char * NTS str) {
  char* COUNT(0) str2 = NTDROP(str);  //cast away the nullterm. This is safe.
  foo(str2);       //KEEP cast: error = Cast from ordinary pointer to nullterm
}

int main() {
  char* NTS hello = "Hello";
  char* NT COUNT(5) str = alloc(char, 6);
  my_memcpy(str, hello, 5);  //This coerces hello to NTS COUNT(5) using strlen
  foo(str);
  cast(str);
  addr(*hello);
  return 0;
}

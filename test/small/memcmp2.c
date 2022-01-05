#include "harness.h"

int (DMEMCMP(1, 2, 3) memcmp)(void* s1, void* s2, int sz);

//Call memcmp on NTS pointers, to make sure that we correctly expand the NT.
int three = 3;
int chars(char* NTS s, int what){
  char *wibble = NTDROP(NTEXPAND("wibble"));
  char *s2 = NTDROP(NTEXPAND(s));
  // Sadly, this doesn't work because we don't allow access to the final 0:
  //memcmp(s,s,4);
  if(what == 0)
    return memcmp(s2,s2,three);
  else
    return memcmp(s2,wibble,3);
}

int six = 3*sizeof(int);
int ints(int* NTS s, int what){
  int *barbara = NTDROP(NTEXPAND(L"barbara"));
  int *s2 = NTDROP(NTEXPAND(s));
  if(what == 0)
    return memcmp(s2,s2,six);
  else
    return memcmp(s2,barbara,3*sizeof(int));
}

int main() {
  char* foo = "foo";
  int (NT bar)[4] = {'b', 'a', 'r', 0};
  if (chars(foo, 0) != 0) E(1);
  if (chars(foo, 1) >= 0) E(2);
  if (ints(bar, 0) != 0) E(3);
  if (ints(bar, 1) != 0) E(4);
  SUCCESS
}

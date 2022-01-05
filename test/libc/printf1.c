
#include <stdio.h>

TESTDEF baseline : success = hello

char a[8];

int main() {
  char * s      = "hello";
  
  printf("int=%02d, float=%f, str=\"%s\"\n", 5, 3.4, "hello");
  printf("char=%c\n", 'a');

  printf("int=%p\n", (int)s);
  
  // TODO: We might want to produce an error in this case.  Deputy inserts
  // a cast from pointer to integer here, which may change the behavior of
  // the code on 64-bit machines.
  printf("int=%d\n", s); // KEEP str_d: success

  printf("str=%s\n", 5); // KEEP : error = Type mismatch

  printf("str=%s\n", a); // KEEP nt1 : error = from ordinary pointer to nullterm

  printf(s, 1); // KEEP nonlit: success = non-literal format string
  
  return 0;
}

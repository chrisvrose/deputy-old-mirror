
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#define E(n) {printf("Error %d\n", n); exit(n); }

TESTDEF : success

int main() {
  struct stat sb1, sb2, sb3;
  
  if(stat("stat1.c", &sb1)) E(1);

  if(lstat("stat1.c", &sb2)) E(2);

  if(sb1.st_size != sb2.st_size) E(3);

  // Try to invoke with a null pointer
IFTEST err : error = non-null check
  stat(0, &sb3);
ENDIF 
  
  return 0;
}

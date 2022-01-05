// To get the crypt definition in unistd.h on Linux, we need this definition.
#define _XOPEN_SOURCE

#include <stdlib.h>
#include <stdio.h>

#if defined(__CYGWIN__)
#include <crypt.h>
#else
#include <unistd.h>
#endif

#define E(n) { printf("Error %d\n", n); exit(n); }
TESTDEF : success


int main() {
  char salt[2] = {'a', 'b'};
  char shortsalt[1] = {'a'};
  char* pwd = crypt("", NTDROP("ab"));
  pwd = crypt("Hello, world.", salt);

IFTEST : error = will always fail
  pwd = crypt("Hello, world.", shortsalt);
ENDIF

    printf("Success\n"); exit(0);
}

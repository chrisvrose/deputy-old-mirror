#include <stdio.h>

TESTDEF baseline : success

char a[] = { 'h', 'e', 'l', 'o' };

int main() {
  char * buff = a; // This should be FSEQ

  fwrite(buff, 1, sizeof(a), stdout); // KEEP baseline
  fwrite(buff, 2, sizeof(a), stdout); // KEEP : error = upper bound

  return 0;
}

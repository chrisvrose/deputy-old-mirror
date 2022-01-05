// String tests from Bill.

#include <stdlib.h>
#include <string.h>

int main() {
  const char * NTS name = "foo!bar";
  char * NT prefix; // prefix is a buffer
  char * NTS suffix;
  int i, len, end;
  
  len = strlen(name);
  end = -1;
  for (i=0; i<len; i++) {
    if (name[i] == '!') {
      end = i;
      break;
    }
  }
  
  if (end < 0) {
    suffix = NULL;
    prefix = strdup(name);
  } else {
    prefix = (char *)malloc(end + 1);
    memcpy(NTDROP(prefix), NTDROP(NTEXPAND(name)), end); // KEEP t1: success
    strncpy(prefix, name, end); // KEEP t2: success
    prefix[end] = 0x00;
    
    suffix = strdup(name + end + 1);
  }
  return 0;
}

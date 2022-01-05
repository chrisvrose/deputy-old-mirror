#include <string.h>      // strerror
#include <stdio.h>       // printf

int main()
{
  char * NTS s = strerror(2);   // file not found
  s++;
  printf("string: %s\n", s);
  return 0;
}

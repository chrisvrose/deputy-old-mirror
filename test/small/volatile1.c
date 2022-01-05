#include <stdlib.h>
#include "harness.h"

//Bug 34.  From TinyOS 

//Don't propagate volatile expressions.  
//We don't want to change this to
//   while (*82 == *82); 
//which will loop forever.
void foo() {
  char now = (int) * (volatile char* SAFE )TC(82U);
  while ( (*(volatile char* SAFE )TC(82U))
         == now) ;
}

//This is one uggggly hack.  The only way I can think of to test this bug
//in a single-threaded app is to grep through the output.

int main() {
  int x = system("grep '^  while.*now' volatile1-tmp.cil.c");
  if (x != 0) {
    printf("Error: grep returned %d\n", x);
    exit(1);
  }
  return 0;
}


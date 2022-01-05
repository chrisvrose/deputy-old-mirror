#include "harness.h"
#include <string.h>    // strchr

char (NT ch)[] = { 'f', 'a', 'r', 'a', 0 };

int main() {
  char * NTS s = &ch[0];  
  char * NTS t = strchr(s, 'a');    
  t+=3;

  if(*t) E(1); // read the 0

  t = strrchr(s, 'a');
  t ++;
  
  if(*t) E(2);

  t = strchr(s, 'b');
  if(t) E(3);
  
  SUCCESS;
}

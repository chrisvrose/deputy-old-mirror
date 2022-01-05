// demonstrate strpbrk problem, and workaround

#include <string.h>     // strpbrk

int main()
{
  char * NTS s = "foo";        
  char * NTS accept = "o";     
  char * NTS w;                

  w = strpbrk(s, accept); 

  return !( *w == 'o' && (w-s == 1) );
}

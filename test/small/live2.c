#include "harness.h"

int a[] = {0,1,2,3,4};

//Dependencies are tricky.  If A depends on B, then an assignment to A
//is a use of B.


int foo() {
  //test that defining q is a use of n.
  int * COUNT(n) q = 0;  //KEEP N: success = Warning: Variable n may be used without being defined
  
  //This is the same test as above, but use memset to set q = 0
  int * COUNT(n) q;          //KEEP memset: success = Warning: Variable n may be used without being defined
  memset(&q,0,sizeof(int*)); //KEEP memset

  //Finally, the declaration of n.
  int n = 5;
  return 0;
}

int main() {
  int * BND(p, p+n) q;
  q = 0;              //KEEP q: error = p may be used without being defined
  int n = 5;
  int * COUNT(n) p = a;
  q = p+2;

  return *q-2;
}

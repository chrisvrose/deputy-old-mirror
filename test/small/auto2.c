
#include "harness.h"

#define FSEQ BND(__this, __auto)
#define SEQ BND(__auto, __auto)

int a[] = { 0, 1, 2, 3};

int * SEQ g;

struct fat {
  int * SEQ s;
  int * FSEQ f;
  int * SEQ as[4];
} gstruct;

int main() {
  int * FSEQ * pf;

  struct fat * FSEQ ps;

  // Test assignment to struct
  gstruct.s = & a[2];
 
  if(gstruct.s[-2] != 0) E(1);
  if(gstruct.s[1] != 3) E(2);
  gstruct.s[-3] = 0; // KEEP low1 : error = Assertion failed
  gstruct.s[2] = 0;  // KEEP high1 : error = Assertion failed

  gstruct.f = gstruct.s;
  
  pf = & gstruct.f;

  if((*pf)[0] != 2) E(11);
  if((*pf)[1] != 3) E(12);
  (*pf)[-1] = 0; // KEEP low2 : error = will always fail
  (*pf)[2] = 0;  // KEEP high2 : error = Assertion failed

  gstruct.as[2] = * pf;

  if(gstruct.as[2][0] != 2) E(21);
  if(gstruct.as[2][1] != 3) E(22);
  gstruct.as[2][-1] = 0; // KEEP low3 : error = Assertion failed
  gstruct.as[2][2] = 0;  // KEEP high3 : error = Assertion failed

  {
    int * FSEQ p = & a[2];

    pf = & p;

    if((*pf)[0] != 2) E(31);
    if((*pf)[1] != 3) E(32);
    (*pf)[-1] = 0; // KEEP low4 : error = will always fail
    (*pf)[2] = 0;  // KEEP high4 : error = Assertion failed
  }
  
  return 0;
}

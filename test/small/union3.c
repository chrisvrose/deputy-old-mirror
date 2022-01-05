//This file is from the CCured regression suite.
//KEEP baseline: success

#include "harness.h"

#define CLEARUNION(u) memset(&u, 0, sizeof(u))

enum tags {
  TAG_ZERO = 0,
};

struct host {
  int tag;    // 0 for integer, 1 for int*, 2 for structure, 3 if tag2 choses
  char tag2;  //If tag is 3, then 0 for int, 1 for int*

  union bar {
    int anint     WHEN(tag == TAG_ZERO);
    int * ptrint  WHEN(tag == 1);
    struct str {
      int * * ptrptr;
    } ptrptr    
         /* missing WHEN */           // KEEP e1: error = Missing WHEN
        WHEN(tag_bad == 5)            // KEEP e2: error = ill-formed
        WHEN(tag == 5) WHEN(tag == 6) // KEEP e3: error = more than one WHEN
IFNTEST e1
IFNTEST e2
IFNTEST e3
        WHEN(tag == 5)
ENDIF
ENDIF
ENDIF
         ;
     int *disj WHEN(tag == 10 || tag == 11);
     int *conj WHEN(tag >= 15 && tag <= 17);

     int int2    WHEN(tag == 3 && tag2 == 0);
     int * ptr2  WHEN(tag == 3 && tag2 == 1);
     int unprotected; //needs no WHEN clause.
  } data;

  //A second union that uses the same tags.
  union foo {
    int fooint       WHEN(!(tag & 1));
    int * fooptrint  WHEN(tag & 1);
  } data2;

  struct {
    int x;
  } somethingelse;      
} g;
  

int x;
int * px = &x;

int one() { return 1; }

int main() {

  g.tag = 0;

  // This is good behavior
  
IFTEST baseline
  g.data.anint = 5;
  x = g.data.anint;
  CLEARUNION(g.data);

  g.tag = 1;
  g.data2.fooptrint = px;
  px = g.data2.fooptrint;
  CLEARUNION(g.data2);

  g.tag = 5;
  g.data.ptrptr.ptrptr = &px;
  x = * * g.data.ptrptr.ptrptr;
  CLEARUNION(g.data);

  g.tag = 3; g.tag2 = 1;
  g.data.ptr2 = px;
  px = g.data.ptr2;

  // This is allowed because we are not reading a pointer
  x = g.data.unprotected;

  // We can take the address of a non-discriminated field
  px = & g.somethingelse.x;

  CLEARUNION(g.data);
  g.tag = 10; px = g.data.disj;
  g.tag = 11; px = g.data.disj;

  CLEARUNION(g.data);
  g.tag = 15;px = g.data.conj;
  g.tag = 16;px = g.data.conj;
ELSE

  CLEARUNION(g.data);
  
  // We cannot access pointers when the tag is wrong
  g.tag = 0; x = g.data.ptrint; // KEEP e4: error = will always fail
  g.tag = 0; *g.data.ptrptr.ptrptr = px; // KEEP e5: error = will always fail
  g.tag = 0; { struct str s = g.data.ptrptr; } // KEEP e6: error = will always fail
  
  px = & g.data.anint; // KEEP e7: error = address of a union field
  // We cannot take the address of a field in a subfield
  { int * * * a = & g.data.ptrptr.ptrptr; } // KEEP e8: error = address of a union field

  g.tag = 12;px = g.data.disj; // KEEP e9: error = will always fail
  
  g.tag = 10;px = g.data.conj; // KEEP e10: error = will always fail
  g.tag = 18;px = g.data.conj; // KEEP e11: error = will always fail
  
  //Don't allow changes to the tag.
  g.tag = 0; g.data.anint = 42; g.tag = 1; //KEEP e12: error = Assertion
ENDIF

  return 0;
}


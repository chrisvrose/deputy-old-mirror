//KEEP baseline: success

#include "harness.h"

#define CLEARUNION(u) memset(&u, 0, sizeof(u))

  union bar {
    int anint          WHEN(tag == 2);
    int * SAFE ptrint  WHEN(tag > 2);
    float afloat       WHEN(buf != 0);
  };
struct host {
  int tag;    // 0 for integer, 1 for int*

  union bar  u;

  int* COUNT(tag) buf;  //Another dependency on tag.
};


struct host global;
int gint;

//Test for union as a local
void local() {
  int tag;
  union bar u;
  int* COUNT(tag) buf = 0;  //Another dependency on tag.
  
  //If we don't clear the union with memset, we will have to notice
  //that "u.anint = 2" is also an initialization of the union.
  CLEARUNION(u);  //KEEP noinit: success
  tag = 2;

  u.anint = 2;

  int x = u.anint;

  // I added a special case for memset to allow taking the address of
  // dependent stuff.  Make sure we check for external dependencies:
  CLEARUNION(tag);           //KEEP addrof: error = address of lval

  u.anint -= x;  //set to 0
  tag = 3;
  u.ptrint = &gint;
 
  u.ptrint = 0;
  tag = 1;
  buf = &gint;
  u.afloat = 3.14159;
}

//Test for union in a struct.
int main() {
  global.buf = 0;

  global.tag = 2;

  global.u.anint = 2;

  int x = global.u.anint;

  global.u.anint -= x;  //set to 0
  global.tag = 3;
  global.u.ptrint = &gint;
 
  global.u.ptrint = 0;
  CLEARUNION(global.u);

  global.tag = 1;
  global.buf = &gint;
  global.u.afloat = 3.14159;

  local();
  
  return 0;
}

//This is roughly union1.c, with a trusted annotation instead of WHENs.

//KEEP baseline: success
#include "harness.h"

  union TRUSTED bar {
    int anint;
    int * SAFE ptrint;
    float afloat;
  } u ;
struct host {
  int tag;    // 0 for integer, 1 for int*

  union bar u;

  int* COUNT(tag) buf;  //Another dependency on tag.
};


struct host global;
int gint;

//Test for union as a local
void local() {
  int tag;
  union bar u;
  int* buf = 0;
  
  u.anint = 2;
  tag = 2;

  tag = 3;
  u.ptrint = &gint;
 
  u.ptrint = 0;
  tag = 1;
  u.ptrint = &u.anint;
  if (u.ptrint != &u.ptrint) E(1);
  u.afloat += 3.14159;
}

//Test for union in a struct.
int main() {
  global.u.anint = 2;

  global.tag = 3;
  global.u.ptrint = &gint;
 
  global.u.ptrint = 0;

  global.tag = 1;
  global.buf = &gint;
  global.u.afloat = 3.14159;

  local();
  
  return 0;
}

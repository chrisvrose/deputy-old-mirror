
#include "harness.h"

// Test global initializers
TESTDEF baseline : success


int x, a[8];

int * NONNULL gnonnull = &x;

int * FSEQ px = & x;

int * SEQ px1 = 0;

int * NONNULL g1; // KEEP nonnull : error = non-null 

int * COUNT(3) g2 = & a[4];
int * COUNT(4) g3 = & a[5]; // KEEP ubound1 : error = will always fail
int * SAFE g4 = & a[-1]; // KEEP lbound1 : error = will always fail

int * NT g5 = & a[3]; // KEEP nt : error = from ordinary pointer to nullterm

// Try an array of non-null pointers without enough initializers
int * NONNULL arr1[4] = { &x, &x }; // KEEP nonnull2: error = non-null

struct opt {
  char (NT shname)[8];
  int * NTS lgname;
  char pad[4];
} options[2] = {
  { { 0, 1, 2, 3, 4, 5, 6, 7} } // KEEP nt2 : error = CWriteNT
  { { }, &x } // KEEP nt3 : error = from ordinary pointer to nullterm
};
  
  
int main() {
  return 0;
}

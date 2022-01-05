
#include "harness.h"

struct fat {
  int * SNT _e;
  int * BND(__this, _e) _p;
};

int array[8] = { 0, 1, 2 };

int main() {
  struct fat t;
  
  t._e = array+8; t._p = array;      // KEEP sep : success
  t = (struct fat){array + 8, array}; // KEEP bundle : success

  return t._p[2] - 2;
}

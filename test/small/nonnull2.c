#include "harness.h"

int x = 0;
int * pp = &x;

int deref (int * NONNULL p) {
  return *p;
}

int main() {
  int * NONNULL p = pp; // KEEP hasinit: success

  // This gets initialized to 0, which is illegal because it's NONNULL.
  // (We also get an error because it's used before being defined.)
  int * NONNULL p; // KEEP missinginit: error = p may be used without being defined

  // An explicit assignment is also caught
  int * NONNULL p = 0; // KEEP init: error = will always fail.

  return deref(p);
}

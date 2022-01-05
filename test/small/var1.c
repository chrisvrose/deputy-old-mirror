#include "harness.h"

void foo(int * BND(p, q) p, int * BND(q, q) q) {
    p = p + 1;
}

int main() {
  int * BND(__this, __this+2) p = alloc(int, 2);
  foo(p,p+2);
  return 0;
}

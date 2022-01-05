#include "harness.h"

void bar(int * BND(b,e) q, int * BND(b,b) b, int * BND(e,e) e) {
}

void foo(int * COUNT(n) p, int n) {
    bar(p + 5, p, p + n);
}

int main() {
  int * COUNT(20) p = alloc(int, 20);
  foo(p, 20);
  return 0;
}

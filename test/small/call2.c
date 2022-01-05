#include "harness.h"

void bar(int * SAFE q) {
}

void foo(int * SAFE p) {
    bar(p);
}
int main() {
  int * SAFE p = alloc(int, 1);
  foo(p);
  return 0;
}

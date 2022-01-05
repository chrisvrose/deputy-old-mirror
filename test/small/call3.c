#include "harness.h"

void bar(int * SAFE q) {
}

void foo(int * COUNT(n) p, int n) {
    bar(p + 1);
}
int main() {
  int len = 42;
  int * COUNT(len) p = alloc(int, len);
  foo(p, len);
  return 0;
}

#include "harness.h"

struct bar {
    int * SAFE p;
};

void foo(struct bar * SAFE b) {
    int i;
    i = *b->p;
}

int main() {
  struct bar * SAFE b = alloc(struct bar, 1);
  b->p = alloc(int, 1);
  foo(b);
  return 0;
}

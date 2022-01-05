#include "harness.h"

int* COUNT(2) bar(int * COUNT(3) q) {
  return q;
}

int main() {
  int * COUNT(4) p = alloc(int, 5);
  int* SAFE q = bar(p);
  return *q;
}

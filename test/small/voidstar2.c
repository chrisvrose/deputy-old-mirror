#include "harness.h"

int *SAFE * COUNT(5) global;

void foo() {
  void * tmp = malloc(sizeof(*global) * 5);
  global = tmp;
}

//Infer that the void* in the cast should be "int *SAFE * COUNT(5)."
void bar() {
  int *SAFE * COUNT(5) local = malloc(sizeof(*local) * 5);
  global = (void*) local;
}

int main() {
  foo();
  bar();
  return 0;
}

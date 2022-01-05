
#include "harness.h"

// Test function pointers.

TESTDEF baseline : success

int * COUNT(n) dup(int n, int * COUNT(n) NONNULL old) {
  int* res = (int *)malloc(n * sizeof(int));
  for (int i = 0; i < n; i++) {
    res[i] = old[i];
  }
  return res;
}

//Three functions with slightly different signatures than dup:
int * SAFE foo(int n, int * COUNT(n) NONNULL old) {
  return 0;
}
int * COUNT(n) bar(int n, int * COUNT(n) old) {
  return 0;
}
int * COUNT(n) qux(int n, int * COUNT(n) NONNULL old, int extraarg) {
  return 0;
}


int * COUNT(n) (*funcPtr_1)(int n, int * COUNT(n) NONNULL old) = 0;

typedef int * COUNT(n) (funcType)(int n, int * COUNT(n) NONNULL old);

// We should be able to apply the NONNULL attribute to functions
funcType* NONNULL funcPtr_2
     = &dup   //DROP missinginit: error = non-null check will always fail
     ;

int main() {
  int n = 4;
  int old[] = {0, 1, 2, 3};
  funcPtr_1 = &dup;   //DROP null-function-ptr: error = non-null check

  int* p = (*funcPtr_1)(n, old);
  if (p[3] != 3) E(3);
  p[4] = 2;        //KEEP ubound: error = will always fail

  funcPtr_2 = dup;
  funcPtr_2 = foo; // KEEP wrongType1: error = Type mismatch
  funcPtr_2 = bar; // KEEP wrongType2: error = Type mismatch
  funcPtr_2 = qux; // KEEP wrongType3: error = Type mismatch

  p = (*funcPtr_2)(n, old);

  return 0;
}

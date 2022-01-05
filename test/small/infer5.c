#include "harness.h"

int * SAFE foo(int * SAFE p) {
    return p;
}

int main() {
    int * SAFE p = alloc(int, 1);
    int * q = foo(p);
    *q = 0;
    return 0;
}

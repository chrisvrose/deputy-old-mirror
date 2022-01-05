// Verify that we don't propagate constants past address-of.

#include "harness.h"

void foo(int * SAFE * SAFE pp) {
    *pp = alloc(int, 1);
}

int main() {
    int * SAFE p = 0;
    int n;

    foo(&p);
    n = 0;
    n = *p;

    return 0;
}

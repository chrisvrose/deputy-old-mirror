// KEEP baseline: success

#include "harness.h"

void foo(int * SAFE p) {
    p = p + 1; // KEEP size1: error = will always fail
}

int main() {
    int * SAFE p = alloc(int, 1);
    foo(p);
    return 0;
}

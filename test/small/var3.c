// KEEP baseline: success

#include "harness.h"

void foo(int * COUNT(2) p) {
    p = p + 1; // KEEP size1: error = will always fail
}

int main() {
    int * COUNT(2) p = alloc(int, 2);
    foo(p);
    return 0;
}

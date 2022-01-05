// This test checks that we set bounds properly when allocating a weird
// number of elements.

// KEEP baseline: success

#include "harness.h"

int main() {
    int * SAFE p;

    p = alloc(char, 6);
    p[0] = 0;
    p[1] = 0; // KEEP bounds: error = will always fail

    return 0;
}

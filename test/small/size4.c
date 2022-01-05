// Test that locals with SIZE type are handled properly.  Also tests
// allocation of SIZE types.

// KEEP baseline: success

#include "harness.h"

void foo(void * SIZE(n) p, int n) {
}

int main() {
    void * SIZE(10) p = malloc(10);
    void * SIZE(5) q = p;
    foo(q, 5);
    foo(p, 6);
    foo(q, 6); // KEEP e1: error = Assertion
    return 0;
}

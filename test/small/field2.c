// KEEP baseline: success

#include "harness.h"

struct bar {
    int * SAFE p;
};

void foo(struct bar * SAFE b) {
    b->p = b->p + 1; // KEEP size1: error = will always fail
}

int main() {
    struct bar * SAFE b = alloc(struct bar, 1);
    b->p = alloc(int, 1);
    foo(b);
    return 0;
}

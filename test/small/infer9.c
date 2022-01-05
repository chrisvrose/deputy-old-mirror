// This test ensures that metadata is properly set by allocators.

#include "harness.h"

int main() {
    int * p;
    int * SAFE q;

    p = alloc(int, 10);
    q = p;

    return 0;
}

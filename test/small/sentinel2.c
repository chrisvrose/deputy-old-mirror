#include "harness.h"

//q should be inferred to be a Sentinel, not SAFE.

int main() {
    int *  p = alloc(int, 5);
    int * q = p + 5;
    int n = (int)q - (int)p;
    return 0;
}

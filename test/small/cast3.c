#include "harness.h"

int main() {
    int * COUNT(4) p = alloc(int, 4);
    int * COUNT(2) q = p + 1;
    int * SAFE r = (int * SAFE) (int * BND(q, q + 2)) (p + 2);
    return 0;
}

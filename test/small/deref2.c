#include "harness.h"

int main() {
    int * COUNT(10) p;
    int * SAFE * SAFE q;
    p = alloc(int, 10);
    q = alloc(int * SAFE, 1);
    *q = p;
    return 0;
}

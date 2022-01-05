#include "harness.h"

static int * BND(p,q) p;
static int * BND(q,q) q;

int main() {
    int * COUNT(10) tmp;
    p = 0;
    q = 0;
    tmp = alloc(int, 10);
    q = tmp + 10;
    p = tmp;
    return 0;
}

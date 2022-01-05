// This test makes sure that we can properly infer types when the
// right-hand side is a global.

#include "harness.h"

static int len = 10;
static int * COUNT(len) buf;

int main() {
    int *p;
    int len_local = len;
    buf = alloc(int, 10);
    p = buf + 1;
    return 0;
}

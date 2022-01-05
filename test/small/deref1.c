#include "harness.h"

int main() {
    int * SAFE p = alloc(int, 1);
    int i;
    i = *p;

    return 0;
}

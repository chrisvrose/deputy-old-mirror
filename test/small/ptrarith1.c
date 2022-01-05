#include "harness.h"

int main() {
    int * COUNT(5) p = alloc(int, 5);
    int * BND(p, p + 5) q = p + 5;
    int n = q - p;
    return (n == 5) ? 0 : 1;
}

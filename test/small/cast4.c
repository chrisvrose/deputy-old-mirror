// KEEP baseline: success

#include "harness.h"

int main() {
    int * COUNT(10) pi;
    int * COUNT(5) pj;
    char * COUNT(20) pc;

    pi = alloc(int, 10);
    pc = (char * COUNT(20)) pi;
    pj = (int * COUNT(5)) pc;
    pi = (int * COUNT(5)) pc; // KEEP size1: error = Assertion
    pi = (int * COUNT(10)) pc; // KEEP size2: error = Assertion

    return 0;
}

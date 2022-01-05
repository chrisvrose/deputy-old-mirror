// KEEP baseline: success

#include "harness.h"

int * COUNT(10) myalloc() {
    return alloc(int, 10);
}

int main() {
    int * COUNT(10) p;
    int * BND(q, p+10) q;

    // This call should get broken into two statements, one where we
    // assign to a fresh variable and one where we assign this value to p.
    // The checks for the assignment to p should pass.
    p = myalloc();
    q = p;

    // This part should fail, because the assignment to p will violate the
    // restrictions on q.
    p = myalloc(); // KEEP return1: error = Assertion

    return *q;  //use q so that it's live
}

// Here's a random attempt to circumvent Deputy's type system.  It is
// prevented by the substitution Deputy uses during an assignment.

// KEEP baseline: error = Assertion failed in upper bound check

int main() {
    int * BND(b, e) b;
    int * SNT e;

    int j;
    char * SAFE p = &j;
    int i;

    e = &i;
    b = &j; // This assignment should fail.
    b[1] = 0xdeadbeef;

    *p = 0;

    return 0;
}

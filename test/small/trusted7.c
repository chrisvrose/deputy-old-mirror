// Make sure we check the target of a trusted cast for full annotations.

int main() {
    int a[2] = { 0, 42 };
    int * SAFE p = a;

    // Make sure LHS is annotated on trusted casts.
    int * q = a; // KEEP baseline: success
    int * q = TC(p); // KEEP t1: error = Target "q" of trusted cast
    int * COUNT(2) q = (int *) TC(p); // KEEP t2: error = Target of trusted cast

    // It's okay if the LHS is trusted but otherwise unannotated.
    int * TRUSTED r = TC(p);
    int * COUNT(2) s = (int * TRUSTED) TC(p);

    // Self-assignment used to introduce temporaries with no annotations.
    // Make sure this doesn't cause any mysterious errors.
    int * SAFE t = 0;
    t = TC(t);

    return (q[1] == 42) ? 0 : 1;
}

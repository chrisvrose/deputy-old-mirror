// This check ensures that bad casts from void* to some larger type are
// caught at compile time instead of run time.

// KEEP baseline: success

int main() {
    int i;

    int * SAFE p1;
    int * SAFE p2;

    void * SAFE q1;
    void * q2;

    p1 = &i;
    q1 = TC(p1);
    q2 = q1;

    p2 = TC(q1);
    p2 = TC(q2);

    p2 = q1; // KEEP size1: error = will always fail
    p2 = q2; // KEEP size2: error = will always fail

    return 0;
}

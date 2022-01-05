// This check ensures that bad casts from void* to some larger type are
// caught at compile time instead of run time.

// KEEP baseline: success

int main() {
    int i;
    int * SAFE p1;
    int * SAFE p2;
    void * SAFE q;

    p1 = &i;
    q = TC(p1);
    p2 = q; // KEEP size1: error = will always fail

    return 0;
}

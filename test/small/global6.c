// Test automatic bounds for globals.
// KEEP baseline: success

int * BND(__auto, __auto) g;
extern int * BND(__auto, __auto) g; // KEEP t1: success

int main() {
    int a[10];

    g = a;

    g[0] = 42;
    g[9] = 42;

    return 0;
}

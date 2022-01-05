// Test automatic bounds for fields.
// KEEP baseline: success

struct foo {
    int * BND(__auto, __auto) p;
};

int main() {
    struct foo f;
    int a[10];

    f.p = a;

    f.p[0] = 42;
    f.p[9] = 42;

    return 0;
}

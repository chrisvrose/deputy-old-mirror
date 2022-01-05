// Test automatic bounds for fields.
// KEEP baseline: success

void foo(int * BND(__auto, __auto) p) {
    int a[20];

    p[0] = 0;
    p[9] = 42;

    p[19] = 42; // KEEP e1: error = Assertion failed

    p = a;

    p[19] = 42;
}

void baz(int a, int * BND(__auto, __auto) p, int b) {
    p[0] = a;
    p[9] = b;
}

void bar(int * BND(__auto, __auto) p);

int main() {
    struct foo f;
    int a[10];

    foo(a);
    bar(a);
    baz(0, a, 42);

    return 0;
}

void bar(int * BND(__auto, __auto) p) {
    p[0] = 0;
    p[9] = 42;
}

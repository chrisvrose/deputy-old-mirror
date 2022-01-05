// This test checks that we correctly optimize away arithmetic on
// pointers to opaque types.  Otherwise, the compiler will complain
// that it doesn't know how to perform that arithmetic.

struct foo1;

struct foo2 {
    struct foo1 * SAFE p;
};

void bar(struct foo1 * SAFE p) {
}

int main() {
    struct foo1 * SAFE p1;
    struct foo2 * SAFE p2;
    struct foo2 f;

    p1 = 0;

    p2 = &f;
    f.p = 0;

    bar(p1);
    bar(p2->p);

    return 0;
}

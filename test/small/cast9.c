struct foo {
    int * SAFE p; // KEEP baseline: success
    int * SAFE p; // KEEP cast1: error = Type mismatch
    int a;
    int b;
};

struct foo f;

void * SAFE get() {
    f.a = 5;
    return TC(&f);
}

int main() {
    struct foo * SAFE p = TC(get()); // KEEP baseline
    struct foo *p = get();       // KEEP cast1
    struct foo *p = get();       // KEEP cast2: error = Assertion
    struct foo * SAFE p = get(); // KEEP cast3: error = Assertion
    return p->a != 5;
}

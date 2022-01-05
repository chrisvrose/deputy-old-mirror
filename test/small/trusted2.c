struct foo {
    int * SAFE a;
    int * SAFE b;
};

int main() {
    int i = 2;
    int j = 3;

    struct foo f;
    f.a = &i;
    f.b = &j;

    void * SAFE p = TC(&f);
    struct foo * SAFE q = TC(p);

    return (*q->a + *q->b == 5) ? 0 : 1;
}

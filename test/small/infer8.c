struct foo {
    int a;
    int b;
};

int main() {
    struct foo f;
    int * SAFE p;
    struct foo * q;
    struct foo * SAFE r;

    p = &f.b;
    q = (struct foo * SAFE)((char * TRUSTED) p - (int) &(((struct foo *)0))->b);
    r = q;

    return 0;
}

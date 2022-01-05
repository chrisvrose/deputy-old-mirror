typedef struct bar bar;
struct bar {
    int * SAFE p;
    int * COUNT(len) q;
    int len;
};

void foo(bar * SAFE b) {
    int * px = b->p;
    int * qx = b->q;
    int i;
    for (i = 0; i < b->len; i++) {
        *qx++ = *px;
    }
}

int main() {
    int i;
    int a[10];

    bar b;
    b.len = 10;
    b.q = a;
    b.p = &i;

    foo(&b);

    return 0;
}

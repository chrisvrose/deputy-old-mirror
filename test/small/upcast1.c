// KEEP baseline: success

struct foo {
    int *p;
    int a;
};

struct bar {
    struct foo f;
    int *q;
    int b;
};

int main() {
    struct bar b;
    struct bar *bp1 = &b;
    struct foo *fp1 = (struct foo *) bp1;

    struct bar *bp2 = 0;
    struct foo *fp2 = (struct foo *) bp2;

    struct bar b3[2];
    struct bar * COUNT(2) bp3 = b3;
    struct foo * COUNT(2) fp3 = (struct foo * SAFE) bp3; // KEEP e1: error = will always fail
    struct foo * COUNT(2) fp3 = (struct foo * COUNT(2)) bp3; // KEEP e2: error = will always fail

    b.f.a = 0;
    return fp1->a;
}

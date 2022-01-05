// KEEP baseline: success

void foo(void * SIZE(n) p, int n) {
    char *q = (char*) p;
    q[n-1] = 0;
    q[n] = 0; // KEEP e2: error = will always fail
    *p = 0; // KEEP e3: error = mismatch in coercion
}

int main() {
    int i;
    int a[10];
    int *b[10];
    foo(&i, sizeof(int));
    foo(a, sizeof(int) * 10);
    foo(b, sizeof(int*) * 10); // KEEP e1: error = Type mismatch
    return 0;
}

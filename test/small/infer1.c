void foo(int * COUNT(10) p) {
    int * q = p;
    int i;
    for (i = 0; i < 10; i++) {
        q[i] = 0;
    }
}

int main() {
    int a[10];
    foo(a);

    return 0;
}

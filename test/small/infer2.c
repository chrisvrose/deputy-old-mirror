void foo(int * SAFE p) {
    int * q = p;
    int * r = q;
    *r = 0;
}

int main() {
    int i;
    foo(&i);

    return 0;
}

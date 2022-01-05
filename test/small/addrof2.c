void foo(int * SAFE p) {
    *p = 0;
}

int main() {
    int i;
    int * SAFE p = &i;
    foo(p);

    return 0;
}

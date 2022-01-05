void foo(int * SAFE p) {
     // int * p = p;  Not legal code
    *p = 0;
}

int main() {
    int a;
    foo(&a);
    return 0;
}

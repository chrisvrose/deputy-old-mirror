void foo(void * COUNT(n) buf, int n) {
}

int main() {
    int a;
    foo(&a, sizeof(int));
    return 0;
}

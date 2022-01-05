int *COUNT(n) foo(int n) {
    return 0;
}

int main() {
    int i = 42;
    foo(i); // KEEP t1: success
    foo(&i); // KEEP e1: error
    return 0;
}

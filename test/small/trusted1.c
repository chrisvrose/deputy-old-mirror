// This test checks whether trusted casts to void work as expected.

void foo(void * SAFE p) {
}

int main() {
    int * SAFE p = 0;
    foo(TC(p));
    return 0;
}

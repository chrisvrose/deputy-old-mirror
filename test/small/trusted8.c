// Test TC on functions.

int foo(int x) {
    return x;
}

typedef int (* SAFE fptr)(int);

int main() {
    fptr p = TC(foo);
    int * SAFE p = TC(foo);
    return 0;
}

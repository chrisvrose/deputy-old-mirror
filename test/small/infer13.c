// This test verifies that function call returns are instrumented properly
// even when the return value is a structure.

struct foo {
    int a;
};

struct foo bar() {
    struct foo f;
    return f;
}

int main() {
    struct foo f = bar();
    return 0;
}

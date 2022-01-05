// This test ensures that we check the number of arguments to a function.

// KEEP baseline: success

int foo(int a) {
    return a;
}

int main() {
    return foo(0);
    return foo(0, 1); // KEEP e1: error = Function call has too many arguments
}

int foo() {
    return 42;
}

int main() {
    int i = 0;
    int *p = &i;
    *p = foo();
    return (i == 42) ? 0 : 1;
}

typedef int fun(void);

int foo() {
    return 0;
}

int main() {
    fun *fp;
    int n;

    fp = &foo;
    n = fp();

    return n;
}

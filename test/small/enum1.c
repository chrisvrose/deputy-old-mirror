enum foo {
    FOO = 1
};

int main() {
    int a[2];
    enum foo e = FOO;
    a[1] = 0;
    return a[e];
}

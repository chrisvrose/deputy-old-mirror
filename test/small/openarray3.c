// Don't convert zero-length array to flexible array if it's the only field.

struct padding {
    int x[0];
} __attribute__((__aligned((1) << (7))));

struct foo {
    char a;
    struct padding p1;
    char b;
    struct padding p2;
    char c;
};

int main() {
    struct foo f;
    f.a = 0;
    f.b = 1;
    f.c = 2;
    return 0;
}

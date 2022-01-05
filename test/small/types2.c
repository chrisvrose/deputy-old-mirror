struct foo {
    int * BND(b,e) p;
    int * BND(b,b) b;
    int * BND(e,e) e;
};

int main() {
    struct foo * SAFE f = 0;
    return 0;
}

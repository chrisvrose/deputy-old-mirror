struct foo {
    void (* SAFE fn1)(int * COUNT(len) buf, int len);
    void (* SAFE fn2)(int * SAFE p);
};

int main() {
    struct foo * SAFE f = 0;
    return 0;
}

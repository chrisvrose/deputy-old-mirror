struct foo {
    int * SAFE p;
};

int main() {
    struct foo * SAFE f = 0;
    return 0;
}

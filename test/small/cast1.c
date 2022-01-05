struct foo {
    int * SAFE f;
};

int main() {
    int i = (int) &((struct foo * SAFE) 0)->f;
    return 0;
}

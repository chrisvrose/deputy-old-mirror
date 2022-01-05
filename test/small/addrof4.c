// KEEP baseline: success

void foo(int * COUNT(2) p) {
    *p = 0;
    *p = 1;
}

int main() {
    int i;
    int * SAFE p = &i;
    foo(p); // KEEP size1: error = will always fail

    return 0;
}

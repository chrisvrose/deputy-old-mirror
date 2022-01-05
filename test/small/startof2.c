// KEEP baseline: success

int main() {
    int a[10];
    int * COUNT(20) p = a; // KEEP size1: error = will always fail

    return 0;
}

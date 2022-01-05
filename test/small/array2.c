// This test ensures that the appropriate substitution is applied to the
// base type of an array.

int main() {
    int * SAFE a[1];
    int * SAFE p;

    a[0] = 0;
    p = a[0];

    return 0;
}

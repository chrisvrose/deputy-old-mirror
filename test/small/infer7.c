// This test checks for a bug where the second assignment to a variable
// with inferred bounds is checked against the previous bounds, and thus
// fails the check.

int main() {
    int i1;
    int i2;

    int * SAFE p1;
    int * SAFE p2;
    int * q;

    p1 = &i1;
    p2 = &i2;

    q = p1;
    q = p2;

    return 0;
}

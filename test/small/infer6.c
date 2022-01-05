// This is a test where we set the upper bound of q to be p+1, even though
// p = 0. This requires special handling of sentinels.

int main() {
    int * SAFE p;
    int * q;

    p = 0;
    q = p;

    return 0;
}

struct foo
{
    int * BND(__this, end) buf;
    int * SNT end;
};

int main() {
    int a[5];
    struct foo f;
    int *p = a;

    f.end = a + 5;
    f.buf = a;

    int n = f.buf - p; // KEEP t1: success
    int n = *(f.buf++); // KEEP t2: success

    return 0;
}

// KEEP baseline: success

// This test ensures that we search for a fully-null element instead of
// just a null byte.  It also ensures that the length does not appear
// artificially long for the same reason.

int main() {
    int (NT a)[4];
    int * NTS p;
    int * COUNT(1) NT q;

    a[0] = 0xffffffff;
    a[1] = 0xffffff00;
    a[2] = 0xffffffff;
    a[3] = 0;

    p = a;
    p[2] = 0xdeadbeef;
    p[4] = 0xdeadbeef; // KEEP err1: error = Assertion

    p++;
    p++;
    p++;

    q = a;
    q++;
    q++;
    q++; // KEEP err2: error = Assertion

    return 0;
}

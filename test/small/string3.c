// This test checks our handling of nullterm arrays.
// KEEP baseline: success

int main() {
    char (NT buf)[10];
    char * COUNT(9) p;  /*inferred NT */
    int i;

    p = buf;
    for (i = 0; i < 9; i++) {
        p[i] = 'a';
    }
    p[9] = 0;

    p[9] = 'a'; // KEEP nullwrite1: error = Assertion
    buf[9] = 'a'; // KEEP nullwrite2: error = Assertion
    return 0;
}

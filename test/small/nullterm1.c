// KEEP baseline: success

int main() {
    int * SAFE x;
    int * SAFE (NT a)[10];
    int * SAFE y;

    int * SAFE * COUNT(9) NT p;
    int * SAFE * NTS q;
    int i;
    int sum;

    x = &i; y = &i; // Non-zero guards above and below array

    p = a;
    for (i = 0; i < 9; i++) {
	p[i] = &i;
    }
    p[9] = 0;
    p[9] = &i; // KEEP write1: error = Assertion

    q = p;

    while (*q != 0) {
        sum += **q;
	q++;
    }

    q++; // KEEP inc1: error = Assertion

    return 0;
}

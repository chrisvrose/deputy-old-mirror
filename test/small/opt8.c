// The errors below should be caught at compile time.

// KEEP baseline: success

static int glen = 0;
static int * COUNT(glen) gbuf = 0;

int inc(int * COUNT(len) NONNULL buf, int len) {
    len += 10; // KEEP e3: error = will always fail
    //Use buf so that it's live on the line above.
    return *buf;
}

void ginc() {
  // We can't catch this statically, because gbuf might be NULL.
  glen += 10; // KEEP e4: error = Assertion failed
}

int main() {
    int len;
    int * COUNT(len) buf;
    int a[10];

    len = 10;
    buf = a;

    len += 10; // KEEP e1: error = will always fail

    inc(buf, len);

    glen = 10;
    gbuf = a;

    glen += 10; // KEEP e2: error = will always fail

    ginc();

    return 0;
}

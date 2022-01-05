// This test ensures that abstract types are treated correctly.
// In particular, we need to avoid errors involving arithmetic on
// such types.

struct foo;

struct foo *getfoo() {
    return 0;
}

int main() {
    struct foo *p = getfoo();
    return 0;
}

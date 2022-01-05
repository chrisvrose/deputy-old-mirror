// Misc tests for proper auto behavior.

struct foo {
    int f;
};

int main() {
    int * SAFE p;
    struct foo f;
    // Make sure we don't add new variables inside sizeof.
    int n = sizeof(*((int * SEQ)p));
    // Also make sure we handle auto bounds properly inside addrof.
    int m = &((struct foo * SAFE) TC(&f))->f;
    return 0;
}

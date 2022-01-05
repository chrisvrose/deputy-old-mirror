// Test inference with casts and memcpy.

struct foo {
    char a, b, c, d;
};

void * (DMEMCPY(1, 2, 3) my_memcpy)(void *dst, void *src, int sz) {
    return dst;
}

int main() {
    struct foo foo;
    char p[4];
    // The following cast should *not* be inferred safe!
    my_memcpy((char*)&foo, p, sizeof(foo));
    return 0;
}

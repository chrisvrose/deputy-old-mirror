// This test makes sure we don't strip off the void* -> char* cast
// inside checks, which causes gcc to complain.

void foo(void * SIZE(len) pv, int len) {
    char *pc = pv;
    int i;
    for (i = 0; i < len; i++) {
        pc++;
    }
}

int main() {
    char s[3] = { 42, 42, 42 };
    foo(s, 3);
    return 0;
}

// KEEP baseline: success

void foo(char * COUNT(len) buf, int len) {
}

int main() {
    int a[4];
    {
	TRUSTEDBLOCK // KEEP t1: success
	foo((char *) a, sizeof(a));
    }
    return 0;
}

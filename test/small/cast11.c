struct foo {
	int a;
	int b;
};

int main() {
	struct foo st;
	char * s1 = &st;
        char * COUNT(8) s2 = s1;

	return 0;
}

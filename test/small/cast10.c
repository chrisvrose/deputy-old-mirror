// zf: this is from similar code in isofs/rock.c

struct S {
	char * COUNT(len1) s1;
	char * COUNT(len2) s2;
	unsigned char * COUNT(len3) s3;
	int len1;
	int len2;
	int len3;
};

int main() {
	struct S s;
	// This works.
	s.s1 = s.s2;
	// This should also work but doesn't
	s.s3 = s.s1;
	return 0;
}

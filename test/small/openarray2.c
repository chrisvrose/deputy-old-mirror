// Same as openarray1, but with nullterm.

// KEEP baseline: success

#include "harness.h"

struct S {
	int len;
	char (NT COUNT(len) name)[]; // KEEP t1: success
IFNTEST t1
	char (NT COUNT(len) name)[0];
ENDIF
};

void foo(char * COUNT(n) s, int n) {
}

int main() {
	struct S *p;
	p = malloc(1); // KEEP e2: error = open array allocation
IFNTEST e2
	p = malloc(sizeof(struct S) + 6);
ENDIF
	p->len = 5;
        p->name[4] = 'a';
	p->len = 4;
	p->len = 5;
	p->len = 6; // KEEP e1: error = upper bound
	foo(p->name, p->len);
	return 0;
}

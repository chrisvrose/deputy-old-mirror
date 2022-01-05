// KEEP baseline: success

#include "harness.h"

struct S {
	int len;
	int* (COUNT(len) data)[]; // KEEP t1: success
IFNTEST t1
	int* (COUNT(len) data)[0];
ENDIF
};

void foo(int** COUNT(n) s, int n) {
}

int main() {
	struct S *p;
	p = malloc(1); // KEEP e2: error = open array allocation
IFNTEST e2
	p = malloc(sizeof(struct S) + 5 * sizeof(int));
ENDIF
	p->len = 5;
	p->len = 4;
	p->len = 6; // KEEP e1: error = upper bound
	foo(p->data, p->len);
	return 0;
}

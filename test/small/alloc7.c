#include "harness.h"

struct node {
    struct node * next;
    int * COUNT(len) buf;
    int len;
};

int main() {
    struct node n[2];
    int i;

    for (i = 0; i < 2; i++) {
	struct node *p = &n[i];
	int *tmp;

	p->len = i + 1;
	tmp = malloc(p->len * sizeof(int));
	p->buf = tmp;
    }
}

#include "harness.h"

struct foo {
    int * SAFE p;
};

void bar(struct foo *fp) {
    fp->p = malloc(sizeof(int));
    *fp->p = 0;
}

int main() {
    struct foo f;
    struct foo *fp = &f;

    fp->p = 0;
    bar(fp); // We want to test whether fp->p == 0 is killed here.
    return *fp->p;
}

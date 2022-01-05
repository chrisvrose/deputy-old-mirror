// Makes sure allocations are properly initialized.

#include "harness.h"

void stack_garbage() {
    int a[10];
    a[0] = 42;
    a[9] = 42;
}

void stack_test1() {
    int *a[10];
    if (a[0] != 0) E(1);
    if (a[9] != 0) E(2);
}

void stack_test2() {
    int (NT a)[10];
    if (a[9] != 0) E(3);
}

void * (DALLOC(10 * sizeof(int)) my_alloc)() {
    static int a[10];
    a[0] = 42;
    a[9] = 42;
    return a;
}

void alloc_test1() {
    int **p = my_alloc();
    if (p[0] != 0) E(4);
    if (p[9] != 0) E(5);
}

void alloc_test2() {
    int * NT p = my_alloc();
    if (p[9] != 0) E(6);
}

void * (DREALLOC(p, 20 * sizeof(int)) my_realloc)(void *p) {
    static int a[20];
    int *old = (int * COUNT(10)) TC(p);
    int i;
    for (i = 0; i < 10; i++) {
        a[i] = old[i];
    }
    a[19] = 42;
    return a;
}

void realloc_test1() {
    int **p = my_alloc();
    int **q;
    int i;
    p[0] = &i;
    p[9] = &i;
    q = my_realloc(p);
    if (q[0] != &i) E(7);
    if (q[9] != &i) E(8);
    // TODO: Currently don't test zeros in "new" memory.
}

void realloc_test2() {
    int * NT p = my_alloc();
    int * NT q;
    int i;
    p[0] = 1337;
    p[8] = 1337;
    q = my_realloc(p);
    if (q[0] != 1337) E(9);
    if (q[8] != 1337) E(10);
    if (q[9] != 0) E(11);
    if (q[19] != 0) E(12);
}

int main() {
    stack_garbage();
    stack_test1();

    stack_garbage();
    stack_test2();

    alloc_test1();
    alloc_test2();

    realloc_test1();
    realloc_test2();

    return 0;
}

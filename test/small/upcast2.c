// Need to make sure that upcasts don't take priority over more
// appropriate casts.

#include "harness.h"

struct tcphdr {
    int a; // KEEP t1: success
    short a; // KEEP t2: success
    int b;
};

void *get() {
    return malloc(sizeof(struct tcphdr));
}

int main() {
    struct tcphdr *th = (struct tcphdr * SAFE) TC(get());
    *(((short *)th) + 1) = 0;
    return 0;
}

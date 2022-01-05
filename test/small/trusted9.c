#include "harness.h"

struct foo {
    int len;
    char (TRUSTED blah)[];
};

int main() {
    struct foo* SAFE f;
    f = malloc(sizeof(struct foo) + 10);
    f->len = 10;
    return 0;
}

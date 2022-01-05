// A minimized optimizer bug from writev1.c.  Make sure we don't
// delete the "dead" variable p2.

#include "harness.h"

int main() {
    char *p1;
    char *p2;
    const int size = 8;
    p1 = malloc((size+1) * sizeof p2[0]);
    p2 = p1 + size;
    return 0;
}

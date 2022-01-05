// KEEP baseline: success

#include <string.h>
#include <stdio.h>

int main() {
    char (NT foo)[10];
    int i;

    foo[8] = 42;
    strncpy(foo, "blah blah blah", 7);

    // We're not allowed to specify 10 here, because strncpy can overwrite
    // the final zero otherwise.
    strncpy(foo, "blah blah blah", 10); // KEEP e1: error = nullterm upper bound

    // Make sure item 8 was not overwritten.
    return (foo[8] == 42) ? 0 : 1;
}

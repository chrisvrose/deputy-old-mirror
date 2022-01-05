// KEEP baseline: success

#include <stdarg.h>

int foo(int n, ...) TRUSTED {
    int i;
    int sum = 0;
    va_list ap;

    va_start(ap, n);
    for (i = 0; i < n; i++) {
        sum += va_arg(ap, int);
    }
    va_end(ap);

    return sum;
}

int main() {
    int * SAFE z = 0;
    int sum = 0;

    sum += foo(0);
    sum += foo(1, 1);
    sum += foo(3, 1, 2, 3);
    sum += foo(1, *z); // KEEP err1: error = non-null

    return (sum != 7) ? 1 : 0;
}

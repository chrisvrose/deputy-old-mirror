// KEEP baseline: success

extern void *(DALLOC(size) malloc)(unsigned int size);
extern void *(DALLOC(size1 * size2) calloc)(unsigned int size1, unsigned int size2);

void *(DALLOC(size) my_malloc)(int dummy, int size) {
    return malloc(size);
}

void *(DALLOC(size1 * size2) my_calloc)(int dummy, int size2, int size1) {
    int size = size1 * size2;
    return malloc(size);
}

int * SAFE NULL = 0;

int main() {
    int * COUNT(10) p;
    int * SAFE z = NULL;

    p = my_malloc(5 * sizeof(int), 10 * sizeof(int));
    p = my_malloc(10 * sizeof(int), 5 * sizeof(int)); // KEEP arg1: error = Assertion
    p = my_malloc(*z, 10 * sizeof(int)); // KEEP arg2: error = Assertion

    p = my_calloc(5, sizeof(int), 10);
    p = my_calloc(10, sizeof(int), 5); // KEEP arg3: error = Assertion
    p = my_calloc(*z, sizeof(int), 10); // KEEP arg4: error = Assertion

    return 0;
}

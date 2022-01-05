// Shows trusted cast problem resulting from disappearing trusted cast.

typedef struct node {
    int *next;
} *nodeptr;

typedef struct cell {
    int stuff;
} *cellptr;

int main() {
    nodeptr p = 0;
    cellptr SEQ c = TC(p);
    return 0;
}

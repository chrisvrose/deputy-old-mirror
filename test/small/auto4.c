// KEEP baseline: success

void foo(char * SEQ * SAFE p) {
}

int main() {
    char * SEQ p;
    char * COUNT(0) q;

    foo(&p);
    foo(&q); // KEEP e1: error = nice error message
}

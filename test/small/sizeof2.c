// This test checks that we can use sizeof on any variable that is
// currently in scope.

char a[5];

typedef char c[5];

int main() {
    char b[5];

    char * COUNT(sizeof(a)) pa;
    char * COUNT(sizeof(b)) pb;
    char * COUNT(sizeof(c)) pc;

    pa = a;
    pb = b;
    pc = b;

    return 0;
}

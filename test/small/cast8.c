// This test makes sure that we can cast between pointers to signed and
// unsigned data.

int main() {
    char * SAFE pc;
    unsigned char * SAFE qc;

    int * SAFE pi;
    unsigned int * SAFE qi;

    pc = 0;
    qc = pc;

    pi = 0;
    qi = pi;

    return 0;
}

//This test should fail
//KEEP baseline: error = address of lval

int main() {
    int * BND(r,r) p = 0;
    int * SAFE r = 0;
    int * SAFE * SAFE q = &p;

    return 0;
}

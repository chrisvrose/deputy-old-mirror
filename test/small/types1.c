//This test should fail
//KEEP baseline: error = Type of variable q is ill-formed

int main() {
    int * SAFE r = 0;
    int * BND(r,r) * SAFE q = 0;

    return 0;
}

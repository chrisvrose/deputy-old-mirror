//This test should fail
//KEEP baseline: error = address of lval

int main() {
    int i;
    int * COUNT(i) q = 0;
    int * SAFE p = 0;
    p = &i;

    return 0;
}

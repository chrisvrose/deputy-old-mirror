//This test should work before any changes.
//KEEP baseline: success

int * SAFE p;
int * BND(q,p) q;       //KEEP dep: success

int main() {
    q = 0;              //KEEP dep
    p = 0;
    return 0;
}

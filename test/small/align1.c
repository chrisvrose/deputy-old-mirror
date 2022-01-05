int main() {
    int a[5];
    int * COUNT(5) b = a;
    int * SNT e = b + 5;

    int * BND(b, e) p = b + 2;

    int * SNT b1 = b + 1;
    int * SNT e1 = b + 4;

    int * BND(b1, e1) p1 = p;

    int * SNT b2 = (int * SNT)((char * COUNT(20)) b + 2);
    int * SNT e2 = (int * SNT)((char * COUNT(20)) b + 18);
    int * SNT e2 = (int * SNT)((char * COUNT(20)) b + 18);

    int * BND(b2, e2) p2 = p; // KEEP align1: success
 
    //we'll allow misaligned pointers, if they aren't dereferenced
    int * BND(b2, e2) p3 = b+4;
    int x = *p3;                // KEEP align2: error = will always fail

    return 0;
}

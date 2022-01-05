int main() {
    int i;
    int * BND(p, __auto) p;
    int * BND(p, p+1) q;

    p = &i;

    q = p;

    // The test fails here because we insert the following code when
    // assigning the automatic bounds variable:
    //  p = 0;
    //  p_e = p+1;
    //  p = q;
    //matth: but it works now that we don't do p = 0
    p = q;
    
    return 0;
}

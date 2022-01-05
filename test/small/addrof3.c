// KEEP baseline: success

int main(int argc, char** argv) {
    int i;
    int two = argc+1;
    int * SAFE p = &i; // KEEP baseline
    //Assign &i to a pointer of type COUNT(2).  We'll catch this at
    // either compile time or run time
    int * COUNT(2) p = &i; // KEEP size1: error = will always fail
    int * COUNT(two) p = &i; // KEEP size2: error = Assertion
    *p = 0;

    return 0;
}

//This test should fail
//  When using interprocedural inference, the typedef is unrolled, so we
//  get a different error.   was:  Type of typedef foo is ill-formed
//KEEP baseline: error = Error: Type of 

typedef int * BND(b,e) foo;

int main() {
    int i;
    foo f = &i;
    return 0;
}

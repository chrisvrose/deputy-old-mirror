// KEEP baseline: success

#include "harness.h"

//Tests for mixing auto with trusted blocks.  For each case when auto can
//occur, make sure trusted code doesn't assign to auto lvalues.

int * globSafe;
int * FSEQ globSeq;


struct test {
  int* fieldSafe;
  int* SEQ fieldSeq;
};

//Trusted functions behave the same as trusted blocks, (except that the types
//of locals are also trusted.)
void structtest(struct test* p) TRUSTED {
  p->fieldSafe = (int * SAFE) 10;
  p->fieldSafe = (int * SAFE) p->fieldSeq;
  p->fieldSeq = (int * SAFE) 10; // KEEP struct: error = Trusted code assigns to
  p->fieldSafe = &p->fieldSafe;
  p->fieldSafe = &p->fieldSeq; //KEEP struct2: error = In trusted code, you may not take the address
}

//We support auto bounds in the formals of trusted functions.  This follows
//from the general rule that trusted functions can read
void formalTest(int* SAFE p, int * SEQ q) TRUSTED {
  
  if (p != q) exit(-1);

  p = (int * SAFE) 10;
  p = q;
  q = (int * SAFE) 10; // KEEP formal: error = Trusted block assigns to "q"
  p = &p;
  p = &q;              //KEEP formal2: error = address of a formal
        
  //From trusted code, it's not okay to call a function with auto bounds in
  //its formals.
  if (p != 0) formalTest(0,q); //KEEP badcall: error = Calling function formalTest from trusted code
}

int main() {
    int * SAFE p;
    int * SEQ q = 0;
    { TRUSTEDBLOCK
        p = (int * SAFE) 10;
        p = q;
        q = (int * SAFE) 10; // KEEP local: error = Trusted block assigns to "q"
        p = &p;
        p = &q;              //KEEP local2: error = In trusted code, you may not take the address of
    }
    { TRUSTEDBLOCK
        globSafe = (int * SAFE) 10;
        globSafe = globSeq;
        globSeq = (int * SAFE)10; //KEEP global: error = Trusted code assigns to "globSeq
        p = &globSafe;
        p = &globSeq;             //KEEP global2: error = In trusted code, you may not take the address of
    }
    { TRUSTEDBLOCK
        int * SAFE * SAFE pp = malloc(sizeof(*pp));
        int * SEQ * SAFE qq = malloc(sizeof(*qq));

        *pp = (int * SAFE) 10;
        *pp = *qq;
        *qq = (int * SAFE)10;  //KEEP memory: error
    }
    struct test local;
    structtest(&local);
    formalTest(0,q);
    return 0;
}


//Trusted code should also not return fat values.
int * SEQ returnTest(int * SEQ p) {
  if (!p) { TRUSTEDBLOCK  return 0; } //KEEP return: error = Trusted block contains return of fat type
  return p;
}

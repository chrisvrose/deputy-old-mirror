// KEEP baseline: success

int i;
IFTEST e1: error = Casts in global initializers may not have auto bounds
int * SAFE p = (int * SEQ) &i;
ELSE
int * SAFE p = &i;
ENDIF

int * SAFE q = TC(-1);

int main() {
    return (p != q) ? 0 : 1;
}

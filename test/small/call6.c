// KEEP baseline: success

int id(int n) {
    return n;
}

struct foo {
    int  * COUNT(len) data;
    int len;
};

void * (DALLOC(sz) allocate)(int sz) {
  return 0;
}

int main() {
    int a[5];
    struct foo f;

    f.data = 0;
    f.len = id(5);
    f.data = a;
    f.len = id(3);
    f.len = id(10); // KEEP err1: error = Assertion

    f.data = 0; f.data = (int *)allocate(f.len * sizeof(int)); // KEEP err2 : success
    
    return 0;
}

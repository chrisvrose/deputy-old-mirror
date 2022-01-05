
struct foo {
    char * COUNT(len) buf;
    int len;
};

TESTDEF default : success

int main() {
  struct foo f;
  int n;
  char * COUNT(n) p;

  // If we do not use n, then it gets eliminated !!! 
  n = 2; // KEEP : success
  
  f.buf = p;

  return 0;
}

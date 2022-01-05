
// Physical subtyping

struct host {
  struct child {
    struct nephew {
      int data;
    } n;
    int * data1;
  } c;
  int * data2;
} x;

int main() {
  struct child * c = (struct child*) &x;
  struct newphew * n = (struct nephew*) &x;

  if(c != n || c != &x) return 1;

  return 0;
}

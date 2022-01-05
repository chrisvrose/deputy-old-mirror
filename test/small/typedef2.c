

typedef int vector[3];

// When we pass arrays as arguments, CIL turns them into pointers
// Because they are not annotated, they appear as SAFE pointers !!
void foo(vector v) {
  v[0] = 0; v[1] = 1; v[2] = 2;
  
}

int main() {
  vector v;

  v[0] = 0;
  v[1] = 1;
  v[2] = 2;

  foo(v);
  
  return 0;
}


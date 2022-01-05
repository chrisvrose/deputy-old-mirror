
int foo(int * COUNT(n), int n);

// Now change the name of the variables. We must handle this gracefully.
int foo(int * COUNT(m) p, int m) {
  p[2] = 1;
}


int main() {
  int buff[8];
  foo(buff, 8);
  return 0;
}

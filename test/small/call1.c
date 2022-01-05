void bar(int * COUNT(m) q, int m) {
}

void foo(int * COUNT(n) p, int n) {
    bar(p, n);
}
int main() {
  foo(0,10);
  return 0;
}

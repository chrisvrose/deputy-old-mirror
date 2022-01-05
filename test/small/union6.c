
union u {
  struct {
    int x,y,z;
  } a;
  struct {
    int m[3];
  } b;
};

int main() {
  union u u;
  int t = 0;
  u.a.x = 1;
  u.b.m[2] = 2;
  t = u.a.x + u.b.m[2];
  return (t == 3) ? 0 : 1;
}

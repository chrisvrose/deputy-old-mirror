char (NT ntbuf)[10];
int main() {
  char* NT COUNT(9) p = ntbuf;
  char* COUNT(10) q;
  char* z = 0;
  { TRUSTEDBLOCK
      q = p;
      //Inference should not see the ++.  If it does, z will get auto bounds,
      //and this assignment will be illegal in the trusted block.
      z++;
  }
  q[9] = 'a';
  return 0;
}

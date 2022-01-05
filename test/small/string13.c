struct str {
  char (NT data)[32];
} x;

struct nostr {
  char data[32];
} y;


int main() {
  char * NT p = & x.data[0]; // KEEP first : success

  // When we take the address of an element inside an array, maintain the bounds
  char * p = & y.data[5]; p[-5] = 1; p[31 - 5] = 1; // KEEP third : success
  char * p = & y.data[5]; p[-6] = 1; // KEEP lbound : error = Assertion
  char * p = & y.data[5]; p[32-5]  = 1; // KEEP ubound : error = will always fail

  
  return 0;
}


// Test TRUSTED functions

int* global = 0;

void bad1(int a, int *b, int *c) TRUSTED {
  global = a;
}

void (TRUSTED bad2) (int a, int *b, int *c) {
  global = a;
}


// a K&R declaration.  Currently, this won't parse if the return type
// is omitted.
void (TRUSTED old_bad) (a, b, c)
     int a, * c;
     int *b;
{
  global = a;
}

int main() { return 0; }

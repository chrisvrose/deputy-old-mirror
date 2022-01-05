

int array[128];

int main() {
  // I get that uninformative "ill-formed type". This should be allowed
  // and in fact this is not even a dependent type, or dependent only on
  // link time constant
  int * BND(array, array + 8) ptr = &array[6]; // KEEP 1 : success

  // Maybe we ought to extend the syntax for attributes ?
  int * BND(&array[0], &array[8]) ptr = &array[6]; // KEEP 2 : success

  
  *ptr = 0;

  return array[6];
  
}

// Allow values to depend on constant globals. 
//KEEP baseline: success

int array[50];

//Deputy needs to know not to delete ASSUMECONST globals even if they are 
// unused.
static int * ASSUMECONST SNT sentinel1 = array;
static int * ASSUMECONST SNT sentinel2 = &array[50];


struct mystruct {
  int * BND(sentinel1, sentinel2) f;
} globalstruct;

int * BND(sentinel1, sentinel2) foo(int * BND(sentinel1, sentinel2) arg) {
  if (arg) 
    return arg+1;
  else
    return &array[0]; 
}

int main() {
  array[45] = 'a';
  globalstruct.f = &array[42];

  sentinel2 = &array[50]; // KEEP warn : success = Warning: Assigning to an ASSUMECONST value
  
  int * p = foo(globalstruct.f);
  int * p2 = (int * BND(sentinel1, sentinel2))p;
  int * BND(sentinel1, sentinel2) p3 = p2 + 2;


  p3[5] = 'b';                  //KEEP local: error = Assertion failed
  *(globalstruct.f - 43) = 'c'; //KEEP field: error = Assertion failed
  
  //p3 should equal &array[45]
  return *p3 != 'a';
}

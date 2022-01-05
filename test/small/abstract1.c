//Deputy's optimizer ran into trouble here because of the abstract type.

struct abstract;

//in testlib.c:
extern struct abstract* get_abstract();
extern void check_abstract(struct abstract* p);

//We don't yet support annotating abstract pointers with a length (even 1)
//because there's no way to insert runtime checks that don't use arithmetic.
struct abstract* SAFE global;

int main() {
  struct abstract *d;
  int * p = 0;
  
  if ((d = get_abstract()) == ((void *) 0))
    {
    }
  check_abstract(d);   //KEEP baseline: success
  check_abstract(d+1); //KEEP arith: error = Arithmetic on abstract pointer
  check_abstract(p);   //KEEP wrongtype: error = Error: Type mismatch

  global = d;

  return 0;
}


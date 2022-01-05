//Make sure the optimizer doesn't delete labels by mistake.

#include <string.h> //strlen

int main() {
  int type;
  //CIL will insert a label '__Cont' here to implement the continue.  
  for (type = 0; type == 0; ) {
    type = 1;
    continue;
  }
  return 0;
}
  
struct n {
  struct n* n_cdr;
  int n_type;
  char* NTS n_str;
};
typedef struct n NODE;
typedef int FIXNUM;
NODE *cvfixnum(FIXNUM n);

NODE *xlength(NODE *args)
{
    NODE *arg = args;
    int n;
    if (((arg) == 0 || (arg)->n_type == 2))
      arg = ((arg)->n_cdr);
    else if (((arg) && (arg)->n_type == 4))
      n = strlen(((arg)->n_str));
    else if (arg)
      return 0;
}

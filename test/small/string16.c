#include "harness.h"

char * NT check(int arg) {
  char* res =  arg > 0 ? "true" : "false";
  return res;
}

//Try a disconnected node:  We have to look at the ? : to get the type of res, 
//since we can't infer it from the use (vararg functions are not implemented).
void print(int arg) {
  char* res =  arg > 0 ? "true" : "false";
  char* fmt = "%s\n";
  printf(fmt, res);
}

int main() {
  print(1);

  if (check( 1)[0] != 't') exit(1);
  if (check(-2)[0] != 'f') exit(2);

  return 0;
}

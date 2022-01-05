//Yet another set of string tests.

//KEEP baseline: success

#include <string.h>
#include "harness.h"

struct withstring {
  int x;
  char* NTS str;
};

char* NTS global;

int loop() {
  int acc = 0;
  char * s = NTEXPAND(global);
  while (*s) {
    acc += *s;
    s++;
  }
  //we should be able to write an empty string.
  global = s;
  return acc;
}

void copy(char * NTS in) {
  char * NT s = NTEXPAND(in);
  char * NT t = NTEXPAND(global);
  while (*s) {
    *t = *s;
    t++; s++;
  }
  *t = *s;
}

static char (NT terminated)[2] = { 1, 0 };
static char (NT tooshort)[2] = { 1 };   //should be padded with zeros, so OK.
static char notterminated[2] = { 1, 1 }; //KEEP arraynotterm: error = Cast from ordinary pointer to nullterm

void array() { 
  char notterminated_local[2] = { 1, 1 }; //KEEP arraynotterm_local: error = nullterm write check

  copy("HELLO, WORLD");
  copy(tooshort);            //KEEP tooshort: success
  //make sure tooshort[1] == 0
  if (global[1] != 0) E(2);  //KEEP tooshort

  copy(terminated);          //KEEP terminated: success
  copy(notterminated);       //KEEP arraynotterm
  copy(notterminated_local); //KEEP arraynotterm_local
}

int main() {
  char* tmp = malloc(13);
  strncpy(tmp, "Hello, world", 12);
  global = tmp;

  copy("HELLO, WORLD");
  copy("HELLO, WORLD!");  //KEEP ubound: error = nullterm write check

  array();

  loop();
  SUCCESS;
}

#include "harness.h"

char * NT COUNT(49) x;
char * COUNT(50) y;

//Don't let the NT spread through the return type of malloc from to x and y.
int main() {
  x = malloc(50);
  y = malloc(50);
  return 0;
}

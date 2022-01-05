

#include "harness.h"

#define FSEQ BND(__this, __auto)

char (NT str)[] = "test";

char * FSEQ NT gnt;  // Try to conflate a NT with a non NT

char array[] = { 0, 1, 2, 3 };

char * FSEQ    g;

int main() {

  gnt = str;

  g = array; // This will generate a cast from NT to ordinary pointer

  return 0;
}

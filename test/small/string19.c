#include "harness.h"

TESTDEF : success

int BuildWord(char * NTS pchWord)
{
  int i;
  char * NTS pch = pchWord;

  /* original code: */
  while ((i = *pch) != '\0') { pch ++; }

  // This other variant would increment the pointer past the 0
  while ((i = *pch ++) != '\0') { } // KEEP : error
   
  printf("%s\n", pchWord);

  // however, going one past the final null is bad!
  pch++;               // KEEP : error = Assertion
  return i;
}


int main()
{
  static char (NT test)[] = "foo";

  return BuildWord(test);
}

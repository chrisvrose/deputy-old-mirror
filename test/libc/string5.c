// String tests from David.

#include <stdio.h> 
#include <string.h>


IFTEST docstring-1 : success

void separate_short_docstring(char * NT str,
                              char *NT *short_s,
                              char *NT *long_s)
{
  char *dot, *at;

  if(str == NULL) {
    *short_s = NULL;
    *long_s = NULL;
    return;
  }

  /* find the first period, followed by whitespace, or the first '@', preceded by whitespace */
  dot = str;
  do {
    dot = strchr(dot,'.');
    if(dot == NULL) break;
    dot++;
  } while(*dot != '\0'  &&  *dot != ' '   &&  *dot != '\t'   &&  *dot != '\r'   &&  *dot != '\n');

  at = str-2;
  do {
    at = strchr(at+2,'@');
    if(at == NULL) break;
    at--;
    if(at < str) at++;
  } while(*at != ' '   &&  *at != '\t'   &&  *at != '\r'   &&  *at != '\n');

  if(at && at < dot) 
    dot = at;


  /* check for the beginning of the next sentance */
  if(dot != NULL) {
    dot += strspn(dot, " \t\n\r.");
    if( *dot == '\0' ) 
      dot = NULL;
  }

  /* short description only  */
  if(dot == NULL) {
    *short_s = str;
    *long_s = NULL;
  } 

  /* both short and long descriptions */
  else {
    *(dot - 1)= '\0';
    *short_s = strdup(str);
    *(dot - 1)= ' ';
    *long_s = str;
  }
}

void test(char *NT str)
{
  char *shorts, *longs;

  separate_short_docstring(strdup(str), &shorts, &longs);

  printf("long: %s\n  short: %s\n\n", longs ? longs : "<none>", shorts);
}

int main(int argc, char *NT *argv)
{
  test("Simple.");
  test("With long. Longer");
  test("This is a short with a.dot.");
  test("The @marks the beginning of long, but this example reveals a bug");
  test("The @marks the beginning of long. Works with more than one sentence");
  test("But@not always");
  return 0;
}

ENDIF


// ====================================
IFTEST docstring-2  : success

int issep(char c)
{
  return c == '\0' || c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

void separate_short_docstring(char *NT str, char *NT *short_s, char *NT *long_s)
{
  char *dot, *at;

  if (str == NULL)
    {
      *short_s = NULL;
      *long_s = NULL;
      return;
    }

  /* find the first period, followed by whitespace, or the first '@',
     preceded by whitespace */
  dot = str;
  for (;;)
    {
      dot = strchr(dot,'.');
      if(dot == NULL)
	break;
      dot++;
      if (issep(*dot))
	break;
    }

  at = str;
  for (;;)
    {
      at = strchr(at, '@');
      if (at == NULL)
	break;
      if (at > str && issep(at[-1]))
	{
	  at--;
	  break;
	}
      at++;
    }

  if (at && (!dot || at < dot)) 
    dot = at;

  /* move to the beginning of the next sentance */
  if (dot)
    {
      dot += strspn(dot, " \t\n\r.");
      if (*dot == '\0') 
	dot = NULL;
    }

  if (!dot)
    {
      /* short description only  */
      *short_s = str;
      *long_s = NULL;
    } 
  else
    {
      /* both short and long descriptions */
      dot[-1] = '\0';
      *short_s = strdup(str);
      dot[-1] = ' ';
      *long_s = str;
    }
}

void test(char *NT str)
{
  char *shorts, *longs;

  separate_short_docstring(strdup(str), &shorts, &longs);

  printf("long: %s\n  short: %s\n\n", longs ? longs : "<none>", shorts);
}

int main(int argc, char *NT *argv)
{
  test("Simple.");
  test("With long. Longer");
  test("This is a short with a.dot.");
  test("The @marks the beginning of long, but this example reveals a bug");
  test("The @marks the beginning of long. Works with more than one sentence");
  test("But@not always");
  return 0;
}

ENDIF

// ===========================================
IFTEST  int2str : success

/* integers are 31 bits long, in base 2 this makes 31 characters
   + sign + null byte + 1 for luck */
#define INTSTRLEN 34
#define FALSE 0
#define TRUE 1

#include <stdint.h>

typedef uint32_t u32;
typedef int32_t i32;

static char basechars[17] = "0123456789abcdef";

char *int2str(char * str, int base, u32 n, int is_signed)
/* Requires: base be 2, 8, 10 or 16. str be at least INTSTRLEN characters long.
   Effects: Prints the ASCII representation of n in base base to the
     string str.
     If is_signed is TRUE, n is actually an i32
   Returns: A pointer to the start of the result.
*/
{
  char *pos;
  int minus;

  /* ints are 32 bits, the longest number will thus be
     32 digits (in binary) + 1(sign) characters long */
  pos = str + INTSTRLEN - 1;
  *--pos = '\0';

  if (is_signed && (i32)n < 0)
    {
      minus = TRUE;
      if ((i32)n <= -16)
	{
	  /* this is to take care of LONG_MIN */
	  *--pos = basechars[abs((long)n % base)];
	  n = (int32_t)n / base;
	}
      n = -(i32)n;
    }
  else minus = FALSE;

  do {
    *--pos = basechars[n % base];
    n /= base;
  } while (n > 0);
  if (minus) *--pos = '-';

  return pos;
}

void testu(int base, uint32_t n)
{
  char buf[INTSTRLEN];

  printf("%s\n", int2str(buf, base, n, FALSE));
}

void testi(int base, int32_t n)
{
  char buf[INTSTRLEN];

  printf("%s\n", int2str(buf, base, (uint32_t)n, TRUE));
}

int main(int argc, char *NT *argv)
{
  uint32_t i;

  for (i = 0x80000000; i; i >>= 1)
    {
      testu(10, i);
      testi(10, i); testi(10, -i);

      testu(2, i); testu(8, i); testu(16, i);
      testi(10, i + 375);
    }
}

ENDIF


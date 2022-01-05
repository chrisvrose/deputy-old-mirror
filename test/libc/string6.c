// String tests from Ilya.

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/*
 * do_div() is NOT a C function. It wants to return
 * two values (the quotient and the remainder), but
 * since that doesn't work very well in C, what it
 * does is:
 *
 * - modifies the 64-bit dividend _in_place_
 * - returns the 32-bit remainder
 *
 * This ends up being the most efficient "calling
 * convention" on x86.
 */
#if 0
#define do_div(n,base) ({ \
	unsigned long __upper, __low, __high, __mod, __base; \
	__base = (base); \
	asm("":"=a" (__low), "=d" (__high):"A" (n)); \
	__upper = __high; \
	if (__high) { \
		__upper = __high % (__base); \
		__high = __high / (__base); \
	} \
	asm("divl %2":"=a" (__low), "=d" (__mod):"rm" (__base), "0" (__low), "1" (__upper)); \
	asm("":"=A" (n):"a" (__low),"d" (__high)); \
	__mod; \
})
#else
unsigned long do_div_func(unsigned long long * n, unsigned long base) {
  unsigned long long res = *n / base;
  unsigned rem = *n - res * base;
  *n = res;
  return rem;
}
#define do_div(n,base) do_div_func(&(n), (base))
#endif

                                   
/**
 * simple_strtoul - convert a string to an unsigned long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 */
unsigned long simple_strtoul(const char * NTS cp,
                             char * NTS * endp, unsigned int base)
{
  unsigned long result = 0,value;
  
  if (!base) {
    base = 10;
    if (*cp == '0') {
      base = 8;
      cp++;
      if ((toupper(*cp) == 'X') && isxdigit(cp[1])) {
        cp++;
        base = 16;
      }
    }
  } else if (base == 16) {
    if (cp[0] == '0' && toupper(cp[1]) == 'X')
      cp += 2;
  }
  while (isxdigit(*cp) &&
         (value = isdigit(*cp) ? *cp-'0' : toupper(*cp)-'A'+10) < base) {
    result = result*base + value;
    cp++;
  }
  if (endp)
    *endp = (char *)cp;
  return result;
}

                              /**
 * simple_strtoull - convert a string to an unsigned long long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 */
unsigned long long simple_strtoull(const char * NTS cp,
                                   char * NTS * endp,unsigned int base)
{
  unsigned long long result = 0,value;
  
  if (!base) {
    base = 10;
    if (*cp == '0') {
      base = 8;
      cp++;
      if ((toupper(*cp) == 'X') && isxdigit(cp[1])) {
        cp++;
        base = 16;
      }
    }
  } else if (base == 16) {
    if (cp[0] == '0' && toupper(cp[1]) == 'X')
      cp += 2;
  }
  while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
                                                             ? toupper(*cp) : *cp)-'A'+10) < base) {
    result = result*base + value;
    cp++;
  }
  if (endp)
    *endp = (char *)cp;
  return result;
}

/**
 * simple_strtoll - convert a string to a signed long long
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 */
long long simple_strtoll(const char * NTS cp,
                         char * NTS * endp,unsigned int base)
{
  if(*cp=='-')
    return -simple_strtoull(cp+1,endp,base);
  return simple_strtoull(cp,endp,base);
}


static int skip_atoi(const char * NTS * s)
{
  int i=0;
  
  while (isdigit(**s))
    i = i*10 + *((*s)++) - '0';
  return i;
}

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */

// buf - the buffer where to write the number
// end - the last character in buffer towrite ?
// num - the number to convert
// base -
// size - seems to be the size of the representation.
// precision -
// type - 
// Now do we say that the return value is related to the argument ?
static char * number(char * BND(__this, end+1) buf,
                     char * SNT end, unsigned long long num,
                     int base, int size, int precision, int type)
{
  char c,sign,tmp[66]; // This will break if we work with larger numbers
  const char *digits;
  static const char small_digits[] = 
    "0123456789abcdefghijklmnopqrstuvwxyz";
  static const char large_digits[] = 
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int i;
  
  digits = (type & LARGE) ? large_digits : small_digits;
  if (type & LEFT)
    type &= ~ZEROPAD;
  if (base < 2 || base > 36)
    return NULL;
  c = (type & ZEROPAD) ? '0' : ' ';
  sign = 0;
  if (type & SIGN) {
    if ((signed long long) num < 0) {
      sign = '-';
      num = - (signed long long) num;
      size--;
    } else if (type & PLUS) {
      sign = '+';
      size--;
    } else if (type & SPACE) {
      sign = ' ';
      size--;
    }
  }
  if (type & SPECIAL) {
    if (base == 16)
      size -= 2;
    else if (base == 8)
      size--;
  }
  i = 0;
  if (num == 0)
    tmp[i++]='0';
  else while (num != 0)
    tmp[i++] = digits[do_div(num,base)];
  if (i > precision)
    precision = i;
  size -= precision;
  if (!(type&(ZEROPAD+LEFT))) {
    while(size-->0) {
      if (buf <= end)
        *buf = ' ';
      ++buf;
    }
  }
  if (sign) {
    if (buf <= end)
      *buf = sign;
    ++buf;
  }
  if (type & SPECIAL) {
    if (base==8) {
      if (buf <= end)
        *buf = '0';
      ++buf;
    } else if (base==16) {
      if (buf <= end)
        *buf = '0';
      ++buf;
      if (buf <= end)
        *buf = digits[33];
      ++buf;
    }
  }
  if (!(type & LEFT)) {
		while (size-- > 0) {
                  if (buf <= end)
                    *buf = c;
                  ++buf;
		}
  }
  while (i < precision--) {
    if (buf <= end)
      *buf = '0';
    ++buf;
  }

  while (i-- > 0) {
    if (buf <= end) 
      *buf = tmp[i];
    ++buf;
  }
  while (size-- > 0) {
    if (buf <= end)
      *buf = ' ';
    ++buf;
  }
  return buf;
}

TESTDEF number-ok : success
IFTEST number-ok
int main(int argc, char* argv[]) 
{
	char* buf = malloc(18);
	number(buf, buf + 17, 7777777, 10, 10, 5, SIGN);
	return 0;
}
ENDIF

TESTDEF number-fail : error
IFTEST number-fail
int main(int argc, char* argv[]) 
{
	char* buf = malloc(18);
        // We are passing an end pointer that is too far. 
	number(buf, buf + 18, 7777777, 10, 10, 5, SIGN);
	return 0;
}
ENDIF

TESTDEF number-fail2 : error
IFTEST number-fail
int main(int argc, char* argv[]) 
{
	char* buf = malloc(8);
        // The size parameter is too large
	number(buf, buf + 7, 7777777, 10, 10, 5, SIGN);
	return 0;
}
ENDIF

TESTDEF skip-ok : success
IFTEST skip-ok
int main(int argc, char* argv[]) 
{
	const char* buf1 = strdup("123456"); 
	skip_atoi(&buf1);
	return 0;
}
ENDIF

TESTDEF skip-fail : error
IFTEST skip-fail
int main(int argc, char* argv[]) 
{
	char * NT buf = malloc(8);
	memcpy(buf, "12345678", 8);
	skip_atoi(&buf);
	return 0;
}
ENDIF

TESTDEF strtoul-ok : success
IFTEST strtoul-ok
int main(int argc, char* argv[]) 
{
	char* buf = malloc(32);
	char* end; 

	strncpy(buf, "1234\0", sizeof(buf));
	simple_strtoul(buf, &end, 10);
	return 0;
}
ENDIF


TESTDEF strtoul-fail : error
IFTEST strtoul-fail
int main(int argc, char* argv[]) 
{
	char* buf = malloc(4);
	char* end; 

	memcpy(buf, "1234\0", 4);
	simple_strtoul(buf, &end, 10);
	return 0;
}
ENDIF


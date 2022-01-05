//no pointers? then no WHEN clause is needed.
//This is from the CCured regression suite

extern int printf(const char * NTS format, ...);
extern void exit(int);
/* Always call E with a non-zero number */
#define E(n) { printf("Error %d\n", n); exit(n); }


typedef unsigned long ULONG;
typedef long LONG;
#ifdef _GNUCC
typedef long long LONGLONG;
#else
typedef __int64 LONGLONG;
#endif

typedef union _LARGE_INTEGER {
  struct {  
    ULONG LowPart;
    LONG HighPart;
  };  
  struct {
    ULONG LowPart;
    LONG HighPart;
  } u;
    LONGLONG QuadPart;
} LARGE_INTEGER;


int main() {
  LARGE_INTEGER foo;

  foo.LowPart = 3;
  foo.HighPart = 7;

  if (foo.u.LowPart != 3) {
    E(1);
  } 
  if (foo.u.HighPart != 7) {
    E(2);
  } 

  return 0;
}

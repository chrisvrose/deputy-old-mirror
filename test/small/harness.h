// This file contains all of the declarations and macros required for the
// small Deputy tests.

#ifndef HARNESS_H
#define HARNESS_H

extern void* (DALLOC(size) malloc)(unsigned int size);
extern void  (DFREE(p) free)(void *p);
extern void* (DMEMSET(1, 2, 3) memset)(void *p, int c, unsigned int n);
extern unsigned int strlen(const char * NTS str);
extern int printf(const char * NTS format, ...);
extern void exit(int code);

#define alloc(t, n) malloc(sizeof(t) * (n))

/* Always call E with a non-zero number */
#define E(n) { printf("Error %d\n", n); exit(n); }
#define SUCCESS { printf("Success\n"); exit(0); }

#endif // HARNESS_H

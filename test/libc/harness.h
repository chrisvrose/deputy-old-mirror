// This file contains macros for the libc Deputy tests.

#ifndef HARNESS_H
#define HARNESS_H

extern int printf(const char * NTS format, ...);
extern void exit(int);

/* Always call E with a non-zero number */
#define E(n) { printf("Error %d\n", n); exit(n); }
#define SUCCESS { printf("Success\n"); exit(0); }

#endif // HARNESS_H

// glob.c
// test glob() function

#include <glob.h>       // glob, globfree
#include <string.h>     // memset
#include <stdio.h>      // printf
#include <unistd.h>     // chdir



int entry()
{
  glob_t globbuf;
  int flags;
  int err;
  int i;

  // clear the globbuf (will CCured complain?)
  memset(&globbuf, 0, sizeof(globbuf));

  // these are exactly the flags that ftpd uses, so I want to make
  // sure they work first..
  flags = GLOB_BRACE |       // shell-style brace expansion
          GLOB_NOCHECK |     // if no match, yield original pattern
          GLOB_TILDE;        // shell-style ~username expansion

  // call glob to find all *.c names
  err = glob("*.c", flags, NULL /*errfunc*/, &globbuf);
  switch (err) {
    case 0:   // no error
      break;

    case GLOB_NOSPACE:
      printf("glob: out of memory\n");
      return 2;

#ifndef __CYGWIN__
    case GLOB_ABORTED:
      printf("glob: read error\n");
      return 2;

    case GLOB_NOMATCH:
      // not really an error as far as glob is concerned, except that
      // I know there are files matching *.c so something must have
      // gone wrong..
      printf("glob: no matches\n");
      return 2;
#endif
      
    default:
      // interestingly, the libc glob.h header does in fact declare
      // some additional codes, implying they might be returned..
      printf("glob: unknown error code: %d\n", err);
      return 2;
  }

  // print some of what was matched
  printf("got %d matches; printing up to 10:\n", globbuf.gl_pathc);

  for (i=0; i < 10 && i < globbuf.gl_pathc; i++) {
    char const *p = globbuf.gl_pathv[i];
    printf("match %d: %s\n", i, p);
  }

  // free the memory
  globfree(&globbuf);
  
  return 0;
}


int main()
{
  int err;

  // since this is often run from cil/test, move into cil/test/small2
  // (if that's really where we are) for a little less degeneracy
  if (0==chdir("small2")) {
    printf("moved into small2/ for less degeneracy\n");
  }

  err = entry();
  if (err) return err;

  // do it again just to be sure
  err = entry();
  if (err) return err;

  return 0;
}

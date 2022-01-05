// DO NOT CHANGE THIS LINE
// Test that read and readv work.

#include <sys/uio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifndef DEPUTY
#define TC(x) (x)
#define NTDROP(x) (x)
#endif

#define myassert(I) do { \
  if(!(I)) { \
    printf("%s:%d **************** assertion failure\n", __FILE__, __LINE__); \
    abort(); \
  } \
} while(0)

void test_read() {
  const int size = 8;
  char *buf = malloc((size+1) * sizeof buf[0]);
  int in = open("readv1.c", O_RDONLY);
  myassert(buf!=0);
  if (in<0) {
    printf("**** error opening file\n");
    abort();
  }

  // read
  {
  int num_left = size;
  while(num_left) {
    int num_read = read(in, NTDROP(buf+size-num_left), num_left);
    num_left -= num_read;
  }
  buf[size] = '\0';
  myassert(close(in)==0);
  }

  // check it is what we expect.
  myassert(strcmp(buf, "// DO NO")==0);

  printf("success\n");
}

void test_readv() {
  int in;
  const int size = 8;
  struct iovec iov[2];
  int i;
  for(i=0; i<2; ++i) {
    iov[i].iov_len = size;
    iov[i].iov_base = malloc(size * sizeof iov[i].iov_base[0]);
    myassert(iov[i].iov_base != 0);
  }

  in = open("readv1.c", O_RDONLY);
  if (in<0) {
    printf("**** error opening file\n");
    abort();
  }

  // readv
  // NOTE: we assume that it maximally fills the buffers.
  {
  int num_read = readv(in, iov, 2);
  printf("num_read = %d\n", num_read);
  myassert(num_read = 2 * size);
  for(i=0; i<2; ++i) {
    ((char*)(iov[i].iov_base))[size-1] = '\0';
  }
  myassert(close(in)==0);
  }

  // check it is what we expect.
  myassert(strcmp(TC(iov[0].iov_base), "// DO N")==0);
  myassert(strcmp(TC(iov[1].iov_base), "T CHANG")==0);

  printf("success\n");
}

int main() {
  printf("test read\n");
  test_read();
  printf("test readv\n");
  test_readv();
  return 0;
}

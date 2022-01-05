// models.c
// tests of models and wrappers

#include <string.h>     // strchr
#include <assert.h>     // assert
#include <stdio.h>      // printf

TESTDEF : success

void t_strchr()
{
  char buf[40] = "hello world";
  char *p = buf;
  char *q;

  q = strchr(p, 'w');
  assert(q == p+6);
}


void t_strdup()
{
  char c[2] = "a";
  char *p = c;           // nominally safe (?)
  char *q;

  q = strdup(p);         // strdup_fs without the model
  assert(q != NULL);     // use q somehow
}


void t_strpbrk_sff()
{
  char buf[10] = "abcdefghi";
  char *b = buf;
  char *p;

  //p++;   // fseq
  //b++;   // fseq
  p = strpbrk(b, "gfe");
  p++;     // seq?
  p--;
  assert(p == buf+4);
  printf("strpbrk_sff ok\n");
}

// Try an unannotated global buffer
IFTEST : error = Cast from ordinary pointer to nullterm: globuf
  char globuf[10] = "abcdefghi";
ELSE 
  char (NT globuf)[10] = "abcdefghi";
ENDIF
void t_strpbrk_fff()
{
  char *b = globuf;
  char *p;

  b++;   // fseq
  p = strpbrk(b, "gfe");
  assert(p == globuf+4);
  printf("strpbrk_fff ok\n");
}

void t_strtok()
{
  char buf[80] = "xabc def ghi";
  char *b = buf;
  char *p;

  p = strtok(b, " ");
  printf("abc: %s\n", p);
  p = strtok(NULL, " ");
  printf("def: %s\n", p);
  p = strtok(NULL, " ");
  printf("ghi: %s\n", p);
  p = strtok(NULL, " ");
  assert(p == NULL);
}


int main()
{
  t_strchr();
  t_strdup();
  t_strpbrk_sff();
  t_strpbrk_fff();
  t_strtok();

  return 0;
}



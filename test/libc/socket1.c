// sockets.c
TESTDEF succ : success
// test some socket function wrappers (and others..)

#include <sys/types.h>       // setsockopt
#include <sys/socket.h>      // setsockopt, ..
#include <netdb.h>           // getnameinfo
#include <netinet/in.h>      // sockaddr_in

#include <stdio.h>           // printf
#include <stdlib.h>          // exit
#include <string.h>          // strdup

#if defined(__CYGWIN__)
//error: This test case does not work on cygwin because getnameinfo is missing
#endif

void fail(int val)
{
  printf("fail(%d)\n", val);
  exit(val);
}


void t_setsockopt()
{
  int tmp=1;
  setsockopt(0, 0, 0, (char*)&tmp, sizeof(tmp));
IFTEST : error = blah blah ubound
  setsockopt(0, 0, 0, (char*)&tmp + 1, sizeof(tmp));  
ENDIF
}

void t_bind()
{
  struct sockaddr_in addr;
  void *p = &addr;
  bind(0, p, sizeof(addr));
  p = (char*)p+1;
  bind(0, p, sizeof(addr));      // KEEP : error = Ubound
}

void t_accept()
{
  struct sockaddr_in addr;
  void *p = &addr;
  int len = sizeof(addr);
  accept(0, p, &len);
  p = (char*)p+1;
  accept(0, p, &len);            // KEEP : error = Ubound
}

void t_getpeername()
{
  struct sockaddr_in addr;
  void *p = &addr;
  int len = sizeof(addr);
  getpeername(0, p, &len);
  p = (char*)p+1;
  getpeername(0, p, &len);            // KEEP : error = Ubound
}

void t_getsockname()
{
  struct sockaddr_in addr;
  void *p = &addr;
  int len = sizeof(addr);
  getsockname(0, p, &len);
  p = (char*)p+1;
  getsockname(0, p, &len);            // KEEP : error = Ubound
}

#ifndef __CYGWIN__
// CYGWIN does not have getnameifo
void t_getnameinfo()
{
  struct sockaddr_in addr;
  void *p = &addr;
  char host[40];
  char serv[40];
  getnameinfo(p, sizeof(addr), host, 40, serv, 40, 0);
  p = (char*)p+1;
  getnameinfo(p, sizeof(addr), host, 40, serv, 40, 0);     // KEEP : error = Ubound
}
#endif

char * NTS t_strdup()
{
  char buf[7] = "fhello";
  char *p = buf;
  char *q;
  char c;

  q++;     // seq
  p++;     // seq

  c = p[4];   // the 'o', definitely ok
  c = p[5];   // we allow this?
  p[5] = 0;   // this?         
  p[5] = 4;   // KEEP : error = Ubound
  p[5] = 0;   // restore

  q = strdup(p);
  c = q[10];          // KEEP : error = Ubound
  q = strdup(p+10);   // KEEP : error = Ubound
  return q;
}

int main()
{
  t_setsockopt();
  t_bind();
  t_accept();
  t_getpeername();
  t_getsockname();
  t_getnameinfo();
  t_strdup();

  printf("no failure\n");
  return 0;
}



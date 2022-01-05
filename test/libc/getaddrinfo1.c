// getaddrinfo.c
// demonstrate problem calling getaddinfo and then using what
// it returns in subsequent socket calls; this comes from ftpd

// TODO: eliminate spurious warning about missing wrappers
// for getaddrinfo and freeaddrinfo

#include <sys/types.h>      // getaddrinfo, connect
#include <sys/socket.h>     // getaddrinfo, connect
#include <netdb.h>          // getaddrinfo
#include <stdio.h>          // perror
#include <unistd.h>         // read, close
#include <string.h>         // memset, memcpy
#include <stdlib.h>         // malloc

#ifdef __CYGWIN__
error: Cygwin does not support getaddrinfo
#endif

// these two come from ftpd/extern.h
struct sockinet {
  unsigned int  si_family;
  uint16_t      si_port;
};

union sockunion {
  struct sockinet      su_si;
  struct sockaddr_in   su_sin;
  struct sockaddr_in6  su_sin6;
  char dirty_pool[32];     // for playing games later
};

#define SA_LEN(x)                                             \
  (((x)->sa_family == AF_INET6) ? sizeof(struct sockaddr_in6) \
				: sizeof(struct sockaddr_in))


int main(int argc, char * NTS argv[])
{
  int err;                     // error code from getaddrinfo()
  struct addrinfo *res;        // getaddrinfo()'s return value (by reference)
  struct addrinfo hints;       // options passed to getaddrinfo()

  union sockunion data_dest;   // address/port for issuing a connect() call
  //struct sockaddr_in data_dest;  // address/port for issuing a connect() call

  int s;                       // socket
  char buf[80];                // for reading from the socket
  int len;                     // return from read()

  // host/service
  char const *name = "argus.eecs.berkeley.edu";
  char const *service = "13";
  if (argc >= 3) {
    name = argv[1];
    service = argv[2];
  }

  // this struct is used to pass hints to getaddrinfo() about what
  // kind of protocol family to use; here I'm supplying the same
  // things that ftpd does
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = PF_INET;          // IPv4 as opposed to IPv6
  hints.ai_socktype = SOCK_STREAM;    // TCP as opposed to UDP

  // call getaddrinfo() to map a name/port combination to
  // a sockaddr structure (and some other info); the 'res' argument
  // will be set to point to the head of a linked list
  err = getaddrinfo(name, service, &hints, &res);
  if (err != 0) {
    printf("getaddrinfo error %d: %s\n", err, gai_strerror(err));
    return 2;
  }

  // for curiosity's sake, count the number of nodes in the list
  {
    int ct = 0;
    struct addrinfo *p = res;
    while (p) {
      ct++;
      p = p->ai_next;
    }
    printf("getaddrinfo returned %d addresses\n", ct);
  }
  printf("addrlen is %d\n", res->ai_addrlen);

  // new solution: try naming the fields explicitly..
  if (res->ai_addrlen == 16) {                               
    // by taking address of 'su_sin', CCured is pacified
    memcpy(&data_dest.su_sin, res->ai_addr, res->ai_addrlen);
  }
  else if (res->ai_addrlen == 28) {
    // 'su_sin6' works similarly, but now there's a different
    // problem--'ai_addr' is only declared to point at a 'sockaddr',
    // and CCured will again complain about the sizes.  I can't just
    // use 'su_sin', because the memcpy wrapper will catch the length
    // mismatch.  Instead I must use George's mkptr_size trick...
      //
      // I wish my machine had IPv6 support working so I could test this..
    struct sockaddr_in6 *src = (struct sockaddr_in6 *)res->ai_addr;
    memcpy(&data_dest.su_sin6, src, res->ai_addrlen);
  }
  else {
    abort();    // unexpected size
  }

  if (argc == 1) {
    // no arguments explicitly supplied, don't actually do the network
    // connection
    printf("skipping connect() step since arguments were implicit\n");
    return 0;
  }

  // now, use this address (IP address and port number) to establish
  // a network connection
  s = socket(AF_INET, SOCK_STREAM, 0 /*pick the reasonable one?*/);
  if (s == -1) {
    perror("socket");
    return 2;
  }

  if (res->ai_addrlen == 16) {
    if (0!=connect(s, (struct sockaddr *)&data_dest.su_sin,
                   SA_LEN((struct sockaddr *)&data_dest))) {
      perror("connect");
      return 2;
    }
  }
  else if (res->ai_addrlen == 28) {     
    // will this work?  CCured is going to see an apparent cast
    // from sockaddr_in6 to sockaddr.. no.  so now I've apparently
    // fixed it with the extra 4 bytes of padding, but still cannot
    // test it due to my IPv6 situation..
    if (0!=connect(s, (struct sockaddr *)&data_dest.dirty_pool,
                   SA_LEN((struct sockaddr *)&data_dest))) {
      perror("connect");
      return 2;
    }
  }
  else {
    abort();
  }


  // just to be sure this really worked, read the first packet; since
  // I've connected to the daytime socket, this should be the time of
  // day in some ascii representation
  printf("established connection!  first packet:\n");
  len = read(s, buf, 79);
  if (len < 0) {
    perror("read");
    return 2;
  }
  buf[len] = 0;
  printf("%d bytes: %s\n", len, buf);

  // close the connection
  if (0!=close(s)) {
    perror("close");
    return 2;
  }

  // free the linked list that getaddrinfo() returned
  freeaddrinfo(res);

  return 0;
}




// sockunion.c
// demonstrate problem with ftpd's 'union sockunion'

//#include <sys/types.h>      // getaddrinfo, connect
//#include <sys/socket.h>     // getaddrinfo, connect
//#include <netdb.h>          // getaddrinfo
//#include <unistd.h>         // read, close
//#include <stdlib.h>         // malloc

#include <stdio.h>          // perror, printf
#include <string.h>         // memset, memcpy

// stdint.h
typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;

// bits/sockaddr.h
typedef unsigned short int sa_family_t;

// netinet/in.h
typedef uint16_t in_port_t;

// netinet/in.h
typedef uint32_t in_addr_t;
struct in_addr {
  in_addr_t s_addr;
};

// bits/socket.h: 16 bytes
struct sockaddr {
  sa_family_t  sa_family ;
  char         sa_data[14];
};

// netinet/in.h: 2+2+4+8 = 16 bytes
struct sockaddr_in {
  sa_family_t     sin_family ;
  in_port_t       sin_port;
  struct in_addr  sin_addr;

  unsigned char   sin_zero[sizeof (struct sockaddr) -
                           (sizeof (unsigned short int)) -
                           sizeof (in_port_t) -
                           sizeof (struct in_addr)];
};


// netinet/in.h: 16 bytes
struct in6_addr {
  union {
    uint8_t  u6_addr8[16];
    uint16_t u6_addr16[8];
    uint32_t u6_addr32[4];
  } in6_u;
};

// netinet/in.h: 2+2+4+16+4 = 28 bytes
struct sockaddr_in6 {
  sa_family_t      sin6_family ;
  in_port_t        sin6_port;
  uint32_t         sin6_flowinfo;
  struct in6_addr  sin6_addr;
  uint32_t         sin6_scope_id;
};


// ftpd/extern.h: 2+2 = 4 bytes
struct sockinet {
  sa_family_t   si_family;
  uint16_t      si_port;
};

// ftpd/extern.h: max(4,16,28) = 28 bytes
union sockunion {
  struct sockinet      su_si;    // not needed to provoke problem
  struct sockaddr_in   su_sin;
  struct sockaddr_in6  su_sin6;
};


int zero = 0;

int main()
{
  struct sockaddr someAddr;
  struct sockaddr_in6 someAddr6;
  union sockunion data_dest;
  int sz;
  int wanted_size = 16;

  printf("sizeof(sockaddr_in): %d\n", sizeof(struct sockaddr_in));
  printf("sizeof(sockunion): %d\n", sizeof(union sockunion));

    // here's a possibly improved version; unfortunately this might
    // only postpone the playing of games until I deal with 'someAddr'
    // in the full generality that ftpd has, but for now this is my
    // prototype solution
  if (wanted_size == 16) {
    memcpy(&data_dest.su_sin, &someAddr, wanted_size);
  }
  else if (wanted_size == 28) {
    memcpy(&data_dest.su_sin6, &someAddr6, wanted_size);
  }
  else {
    abort();     // unexpected size
  }

  sz = sizeof(&data_dest);
  printf("sizeof(&data_dest): %d\n", sz);

  return 0;
}

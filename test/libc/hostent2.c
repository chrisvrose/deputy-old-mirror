#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h> //exit


#define MANJU "manju.cs.berkeley.edu"
#define E(n) { printf("Error %d\n", n); exit(n); }

#if defined __CYGWIN__ || defined _MSVC
error: Cygwin and MSVC do not support gethostbyname_r
#endif

char* NTS addrToString(char* COUNT(4) addr)
{
  static char (NT buffer)[128];
  sprintf(buffer, "%d.%d.%d.%d", 
	 (unsigned char)addr[0], (unsigned char)addr[1], 
	 (unsigned char)addr[2], (unsigned char)addr[3]);  
  return buffer;
}

// dsw: see
// http://www.gnu.org/manual/glibc-2.2.3/html_node/libc_309.html#IDX1688
// for documentation
int main() {
  struct hostent res, *pres;
  char buf[1024];
  int err, i;
  char addr[4];

  //
  // gethostbyname_r
  //

  //lookup manju by name:
  gethostbyname_r(MANJU, &res, buf, 1024, &pres, &err);
  if(pres == NULL) {
       E(1);
  }
  if(strcmp(pres->h_name, MANJU)) {
       E(2);
  }
  printf("%s\n", pres->h_name);
  for (i = 0; pres->h_aliases[i] != NULL; i++) {
      printf("  alias[%d] is %s\n", i, pres->h_aliases[i]);
  }
  memcpy(addr, pres->h_addr_list[0], sizeof addr);
  printf("%s has address %s\n", 
	 pres->h_name, addrToString(addr));

  //lookup localhost by name:
  gethostbyname_r("localhost", &res, buf, 1024, &pres, &err);
  printf("%s\n", pres->h_name);
  for (i = 0; pres->h_aliases[i] != NULL; i++) {
      printf("  alias[%d] is %s\n", i, pres->h_aliases[i]);
  }
  for (i = 0; pres->h_addr_list[i] != NULL; i++) {
      printf("  address[%d] is %s\n", i, addrToString(pres->h_addr_list[i]));
  }

  //
  // gethostbyaddr_r
  //

  //now look up manju using the address we got earlier.
  printf("test gethostbyaddr_r\n");
  gethostbyaddr_r(addr, 4, AF_INET, &res, buf, 1024, &pres, &err);
  printf("lookup manju(%s): %s\n", 
	 addrToString(addr), 
	 pres->h_name);
  if(strcmp(pres->h_name, "manju.CS.Berkeley.EDU")) {
    E(3);
  }
  for (i = 0; pres->h_aliases[i] != NULL; i++) {
    printf("  alias[%d] is %s\n", i, pres->h_aliases[i]);
  }
  
  //now look up localhost using 127.0.0.1
  printf("test gethostbyaddr_r on localhost\n");
  addr[0] = 127;            // localhost
  addr[1] = 0;
  addr[2] = 0;
  addr[3] = 1;
  gethostbyaddr_r(addr, 4, AF_INET, &res, buf, 1024, &pres, &err);
  printf("localhost: %s\n", pres->h_name);
  for (i = 0; pres->h_aliases[i] != NULL; i++) {
    printf("  alias[%d] is %s\n", i, pres->h_aliases[i]);
  }
  //h_name will probably be "localhost" or "localhost.SOME_DOMAIN"
  if(strncmp(pres->h_name, "localhost", 9)) {
    // On the latest incarnation of manju, the localname is manju
    //    E(4);
  }

  printf("success\n"); 
  return 0;
}


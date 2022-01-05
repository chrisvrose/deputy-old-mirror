
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <netdb.h>

#define HOST "www.cs.berkeley.edu"

TESTDEF : success

int main() {
  struct hostent *res = gethostbyname(HOST);
  char *p = res->h_name;
  int i;
  //does HOST match any alias?
  int lookupSucceeded = strcmp(res->h_name, HOST) == 0;

  printf("Host is %s\n", res->h_name);
  for (i = 0; res->h_aliases[i] != NULL; i++) {
      printf("  alias[%d] is %s\n", i, res->h_aliases[i]);
      if(strcmp(res->h_aliases[i], HOST)== 0) {
          lookupSucceeded = 1;
      }
IFTEST ubound_hostent : error = upper bound check
     printf("Next alias = %s\n", res->h_aliases[i + 2]);
ENDIF  
IFTEST ubound_hostent2 : error = nullterm write check
     res->h_aliases[i + 1] = "foo"; 
ENDIF  
  }
  
  for (i = 0; res->h_addr_list[i] != NULL; i++) {
      printf("  addr[%d] is %u.%u.%u.%u\n", i, 
	     (unsigned char)res->h_addr_list[i][0],
	     (unsigned char)res->h_addr_list[i][1],
	     (unsigned char)res->h_addr_list[i][2],
	     (unsigned char)res->h_addr_list[i][3]);
IFTEST ubound_hostent3 : error = upper bound check
     printf("Next addr = %d\n", res->h_addr_list[i + 2][0]);
ENDIF  
  }
  
  if(!lookupSucceeded) {
    exit(1);
  }
  return 0;
}

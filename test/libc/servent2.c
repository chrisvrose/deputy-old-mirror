#include <string.h>
#include <stdio.h>
#include <netdb.h>

#include "harness.h"

#include <netinet/in.h>

// dsw: see
// http://www.gnu.org/manual/glibc-2.2.3/html_node/libc_309.html#IDX1688
// for documentation
void dump_servent(struct servent *s) {
    int i;
    short short_port;
    printf("%s\n", s->s_name);
    for (i = 0; s->s_aliases[i] != NULL; i++) {
        printf("  alias[%d] is %s\n", i, s->s_aliases[i]);
    }
    short_port = s->s_port;
    printf("%d\n", ntohs(short_port));
    printf("%s\n", s->s_proto);
}

int main() {
  struct servent res, *pres;
  char buf[1024];
  char *name = "qotd";
//    char *name = "www";
  char *proto = "tcp";

  printf("looking up name:%s, proto:%s\n", name, proto);

  // reentrant way
  getservbyname_r(name, proto, &res, buf, 1024, &pres);
  {
      short short_port = pres->s_port;
      if(ntohs(short_port) != 17) E(1);
  }
  dump_servent(pres);

  printf("done\n");

  return 0;
}


#include "harness.h"

//
// Make sure deputy supports the standard trick for offsetof
//


typedef struct {
  char bar[4];
  char data[28];
} message_t;

typedef struct {
  char h[2];
} cc1000_header_t;

cc1000_header_t *f(message_t *amsg)
{
  return (cc1000_header_t * SAFE)(void * TRUSTED)
    ((char *)amsg + ((unsigned )& ((message_t*)0)->data
                     - sizeof(cc1000_header_t )));
}

int main() {
  message_t msg;
  cc1000_header_t * res = f(&msg);
  if ((unsigned)res != ((unsigned)&msg + 2)) E(1);
  return 0;
}

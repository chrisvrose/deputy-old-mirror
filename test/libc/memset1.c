#include <string.h>

int main() {
  unsigned char buff[128];
  
  memset(buff, 0, sizeof(buff)); // KEEP : success

  memset((char*)buff, 0, sizeof(buff)); // KEEP : success
  
}

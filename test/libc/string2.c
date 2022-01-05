#include <stdlib.h>
#include <string.h>

typedef char * NT string;

char * NTS getparam(char * NTS name)
{
    int i, len;
    string def;
    char (NT buf)[128];
    string userv;

    strncpy(buf, name, sizeof(buf) - 1);
    len = strlen(buf);

    userv = (string) malloc(len+1);
    strncpy(userv, buf, len);

    if(strcmp(userv, buf)) exit(1);

    return userv;
}

int main() {
  getparam("a string");

  return 0;
}

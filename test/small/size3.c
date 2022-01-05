// Another SIZE test from David Gay.

typedef unsigned char uint8_t;
char buf[20];

void *SIZE(len) get1(uint8_t len)
{
  if (len <= 20)
    return buf;
  else
    return 0;
}

void *SIZE(len) get2(uint8_t len)
{
  // We'll get a void* temporary here.
  return get1(len);
}

int main()
{
  char *fun = get2(10);

  return fun[0];
}

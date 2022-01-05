typedef struct
{
  char data[1];
} cc1000_header_t;

char array[10];

int main(void)
{
  char * p = &array[5];
  //sizeof is unsigned, so CIL treats this as p += 4294967295
  p = p - sizeof(cc1000_header_t);
  return *p;
}

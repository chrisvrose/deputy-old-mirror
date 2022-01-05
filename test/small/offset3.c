typedef struct {
  char foo[28];
} message_t;

int f(unsigned char x)
{
  if (x > 5 + (unsigned )& ((message_t *)0)->foo)
    return 1;
  else
    return 0;

}

int main() {
  return f(5);
}

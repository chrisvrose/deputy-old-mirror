// Very often I put the NT annotation on char instead of char *, by mistake
// I want a warning.

TESTDEF warn : error = Deputy annotations cannot be placed on this type
char NT * p;
int i;
int  COUNT(0) * pi = &i;

int main() {
  *pi = 0;
  return *pi;
}

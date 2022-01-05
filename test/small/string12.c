typedef char * NTS string;

int matchname(string bind, string name)
{
  // Let Deputy infer bounds
  char * NTS bp, * NTS np;  // KEEP test1 : success
  char * bp, * np;    // KEEP test2 : success
  
  bp = bind;
  np = name;
  while (*bp == *np) {
    bp++;
    np++;
  }
  return (*bp == '=' && *np == 0);
}

int main() {
  return ! matchname("foo=2", "foo");
}

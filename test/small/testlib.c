// This file contains externally-defined symbols needed by test cases.

int identity(int arg) { return arg; }

// For extern1.
char extern1buf[4] = "abc";

char * /* COUNT(extern_count) */ extern_p = extern1buf;
int extern_count = sizeof(extern1buf);



//For abstract1
//Just some functions that take and return pointers that abstract1 doesn't 
//know the layout of.
extern void exit(int code);

struct abstract{
  int data;
};
static struct abstract private = {5};

struct abstract* get_abstract() {
  return &private;
}

void check_abstract(struct abstract* p) {
  if (p != &private) exit(10);
}

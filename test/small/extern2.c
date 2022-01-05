TESTDEF baseline: success

//Test that CIL does not delete extern globals that are only used in
//annotations.

extern char * COUNT(extern_count) extern_p;
extern int extern_count;

int main() {
  extern_p++; //KEEP incr: error = will always fail
  extern_p[2] = 0;
  return extern_p[3];
}

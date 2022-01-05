//KEEP baseline: success

//
//Excercise nested struct offsets.
//

#include "harness.h"

struct bar {
  int n;
  int * COUNT(n) p;
};

struct foo1 {
  int len;
  struct bar* COUNT(len) bars;
};

struct foo2 {
  int data;
  struct foo1 foo1;
};

struct foo2 * COUNT(5) foo2_init() {
  struct foo2 * COUNT(5) f = alloc(struct foo2, 5);
  struct bar* COUNT(len) bars;
  for (int i = 0; i < 5; i++) {
    f[i].data =42;
    bars = 0;
    int len = i+1;
    bars = alloc(struct bar, len);
    for (int j = 0; j < len; j++){
      int len2 = 10;
      bars[j].n = len2;
      bars[j].p = alloc(int, len2);
    }
    f[i].foo1.len = len;
    f[i].foo1.bars = bars;
  }
  return f;
}


int main() {
  struct foo2 * COUNT(5) f = foo2_init();
  int i = 4;  //index into f
  int j = i;  //index into bars
  int k = 9;  //index into p
  i++;   //KEEP i: error
  i= -1; //KEEP ineg: error
  j++;   //KEEP j: error
  k++;   //KEEP k: error

  //Make sure each offset is checked correctly:
  int x = f[i].foo1.bars[j].p[k];
  return 0;
}

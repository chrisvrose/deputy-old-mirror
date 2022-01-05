//KEEP baseline: success

//
//Excercise nested struct/array offsets.
//

#include "harness.h"

struct bar {
  int p[10][50];
};

struct foo1 {
  int len;
  struct bar* COUNT(len) bars;
};

struct foo2 {
  int data;
  struct foo1 foo1;
};

void foo2_init(struct foo2 * COUNT(5) f) {
  struct bar* COUNT(len) bars;
  for (int i = 0; i < 5; i++) {
    f[i].data =42;
    bars = 0;
    int len = i+1;
    bars = alloc(struct bar, len);
    f[i].foo1.len = len;
    f[i].foo1.bars = bars;
  }
}


int main() {
  struct foo2 f[5];
  foo2_init(f);
  struct foo2 * SAFE middle = &f[3]; //test addr of array element.
  
  struct foo2 * COUNT(2) middle1 = &f[3]; //test addr of array element.

  int i = 4;   //index into f
  int j = i;   //index into foo1.bars
  int k = 9;   //first index into bar.p
  int k2 = 49; //second index into bar.p
  i++;    //KEEP i: error
  j++;    //KEEP j: error
  k++;    //KEEP k: error
  k2++;   //KEEP k2: error
  i = -1; //KEEP ineg: error = will always fail
  j = -1; //KEEP jneg: error = will always fail
  k = -1; //KEEP kneg: error = will always fail
  k2 = -1;//KEEP k2neg: error = Assertion failed

  //Make sure each offset is checked correctly:
  int x = f[i].foo1.bars[j].p[k][k2];

  f[i].foo1.bars[j].p[k][k2] = x+1; //KEEP set: success

  middle->data = middle1->data;
  
  return 0;
}

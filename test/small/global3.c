
TESTDEF baseline : success

TESTDEF extern : error ~ (undefined reference)|(Undefined symbols)
extern // KEEP extern
int * COUNT(count) array;
extern // KEEP extern
int count;

int thearray[5];

int main() {
  count = 5;
  array = thearray;
  
  return array[0];
}

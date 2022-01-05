
int count;
int * COUNT(count) pdata;


int data[8];
void fill_data() {
  pdata = 0;
  count = 8;
  pdata = data;

  pdata[0] = 0;
}

int main() {
  count = 0;
  fill_data();

  return data[0]; // KEEP : success
  return pdata[0]; // KEEP : success
}

static char array[16];

static char * SNT end;
static char * BND(__this, end) ptr;

int main() {
    end = array + 16;
    ptr = array;

    ptr += 10;

    *ptr = 5;

    return 0;
}

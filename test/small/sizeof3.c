int main() {
    int *p;
    int x = sizeof(*((int * TRUSTED)p)); // KEEP t1: success
    int x = sizeof(*((int * SEQ)p)); // KEEP t2: success
    return x - sizeof(int);
}

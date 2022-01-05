// This test checks for a bug where the optimizer tried to get the bounds
// of an unsigned long (and failed, obviously).

int main() {
    unsigned long l = 0;
    int * SEQ p = TC(l);
}

extern char (NT extern1buf)[]; // KEEP t1: success

int main() {
    extern char (NT extern1buf)[]; // KEEP t2: success
    extern1buf[0] = 0;
    return 0;
}

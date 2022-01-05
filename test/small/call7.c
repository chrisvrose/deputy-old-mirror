// Makes sure that & works properly with arguments that the return type
// depends upon.

int *BND(__this, rlen) foo(int *cmd, int *rlen) {
    return 0;
}

int main() {
    int rcmd;
    int rlen;
    int *SAFE response;
    response = foo(&rcmd, &rlen); // KEEP t1: success
    response = foo(0, &rlen); // KEEP t2: success
    return 0;
}

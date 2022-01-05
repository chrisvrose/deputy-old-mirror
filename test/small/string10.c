// Allow count annotations in arrays with explicit length.

// CIL will insert the length based on the initializer.
char (NTS foo)[] = "foo"; // KEEP t1: success

// Just for sanity's sake...
char (NTS foo)[4] = "foo"; // KEEP t2: success

int main() {
    return (foo[3] == 0) ? 0 : 1;
}

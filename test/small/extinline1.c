TESTDEF noext : success

// We want this annotation to be merged with the extern inline
extern int myfoo(char * NTS str);

IFTEST ext : success
extern inline
ENDIF
char myfoo(char * str) {
    return str[0];
}

int main() {
    return ('A' != myfoo("A string")) ? 1 : 0;
}

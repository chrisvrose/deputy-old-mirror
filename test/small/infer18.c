// Make sure we don't inadvertently change the type of a MinuSPP.
TESTDEF e1: error = Type mismatch

int main () {
    char *SAFE p;
    char buf[1];
    char *SAFE q = (char *SEQ) (&buf[0] - (char *SEQ) p);
    return 0;
}

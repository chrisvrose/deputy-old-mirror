void strlcpy1(char * COUNT(size - 1) NT dest, char * NTS src, int size) {}
void strlcpy2(char * COUNT(size) NT dest, char * NTS src, int size) {}

int main() {
    char (NT buf) [100];
    char * NTS str = "test string";

    strlcpy1(buf, str, sizeof buf);
    strlcpy2(buf, str, sizeof buf); // KEEP error1: error = Assertion

    return 0;
}

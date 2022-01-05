// KEEP baseline: success

int main() {
    char * NTS s = "test";
    char * COUNT(0) s1 = NTDROP(s);
    char * s2 = NTEXPAND(s);
    char c;

    c = s1[0]; // KEEP err1: error = will always fail

    c = s2[0];
    c = s2[3];
    c = s2[4];
    c = s2[5]; // KEEP err2: error = nullterm upper bound check

    return 0;
}

int main() {
    const char * NTS s = "blah";
    const char * t = NTEXPAND(s);
    const char * NT u = s;
    char c = t[1];
    char d = u[1];
    return 0;
}

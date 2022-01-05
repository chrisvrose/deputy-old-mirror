// Test whether we can increment a nullterm string with inferred bounds.

char foo(const char * NTS t) {
    const char * NT s = t;
    s++;
    return *s;
}

int main() {
    char c = foo("test");
    return (c == 'e') ? 0 : 1;
}

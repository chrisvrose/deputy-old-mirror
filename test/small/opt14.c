int main() {
    char a[] = { 'a', 'b', 'c' };
    char const * COUNT(3) p = a;
    ((char *) p)[1] = 'x';
    return 0;
}

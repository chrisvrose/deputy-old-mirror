typedef int * SAFE foo;

int main() {
    int i;
    foo f = &i;
    return 0;
}

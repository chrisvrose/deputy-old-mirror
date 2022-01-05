int main() {
    int a[2];
    int * SAFE p = a;
    int * q = (int * SAFE)((char * TRUSTED) p + 4);
    int * SAFE r = q;
    return 0;
}

int main() {
    int * SAFE p = 0;
    int * SAFE * SAFE q = &p;

    return 0;
}

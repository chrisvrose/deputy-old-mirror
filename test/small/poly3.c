TV(t) choose(int n, TV(t) a, TV(t) b) {
    return n > 42 ? a : b;
}

int main() {
    int a;
    int b;

    int n = choose(13, (int) 3, (int) 5);
    int *p = choose(50, &a, &b);

    return 0;
}

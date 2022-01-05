TV(t) id(TV(t) p) {
    return p;
}

int main() {
    int i = 0;
    int *p = &i;
    return * (int*) id(p);
}

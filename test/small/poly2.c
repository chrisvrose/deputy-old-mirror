TV(t) deref(TV(t) * p) {
    return *p;
}

int main() {
    int i = 0;
    int *p = &i;
    int **pp = &p;
    return deref(deref(pp));
}

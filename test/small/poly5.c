int foo(int (*fn)(TV(t) data), TV(t) data) {
    return fn(data);
}

int cb_int(int *data) {
    return *data;
}

int cb_void(void *data) {
    // doesn't use data
    return 0;
}

int main() {
    int i = 0;
    foo(cb_int, &i);
    foo(cb_void, (void*) 0);
    return 0;
}

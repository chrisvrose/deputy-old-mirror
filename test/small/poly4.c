struct foo {
    int *q;
    int a;
    int b;
};

int getfoo(struct foo *p) {
    return p->b;
}

int getint(int *p) {
    return *p;
}

int run(int (*fn)(TV(t) p), TV(t) p) {
    return fn(p);
}

struct fioo {
    TV(t) data;
    int (*fn)(TV(t) p);
};

int test1() {
    int i = 24;
    struct foo f = { 0, 0, 42 };
    return run(getfoo, &f) == 42 && run(getint, &i) == 24;
}

int test2() {
    struct fioo TP(struct foo *) fi;
    struct fioo TP(int *) pi;

    int i = 24;
    struct foo f = { 0, 0, 42 };

    fi.data = &f;
    fi.fn = getfoo;

    pi.data = &i;
    pi.fn = getint;

    return fi.fn(fi.data) == 42 && pi.fn(pi.data) == 24;
}

int main() {
    return test1() && test2() ? 0 : 1;
}

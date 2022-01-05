// This test catches a bug in inference dealing with open arrays.

struct fib_nh {
    int a;
};

struct fib_info {
    int fib_nhs;
    struct fib_nh (COUNT(fib_nhs) fib_nh)[0];
};

void fib_info_hashfn(const struct fib_info *fi) {
}

int main(int argc, char **argv) {
    struct fib_info *fi = 0;
    fib_info_hashfn(fi);
    return 0;
}

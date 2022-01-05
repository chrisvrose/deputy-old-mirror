//This test should fail
//KEEP baseline: error = Field p of struct foo is ill-formed

struct foo {
    int * BND(b,e) p;
};

int main() {
    struct foo * SAFE f = 0;
    return 0;
}

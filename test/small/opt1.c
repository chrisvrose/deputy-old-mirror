// Verify that unreachable statements don't cause problems.

int main() {
    int * SAFE p = 0;
    return 0;
    *p = 0;
}

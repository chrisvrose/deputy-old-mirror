// This test results (resulted) in a divide-by-zero error due to the
// zero-length struct.

typedef struct spinlock {
} spinlock_t;

spinlock_t slavecpulocks[1];

int main() {
    int i;
    spinlock_t x;

    for (i = 0; i < 1; i++) {
        *(&slavecpulocks[i]) = x;
    }

    return 0;
}

// This test checks for a bug where non-NT char* arguments to printf were
// automatically converted to NT.

#include "harness.h"

struct sk_buff {
    unsigned char * COUNT(0) head;
};

void skb_over_panic(struct sk_buff *skb) {
    printf("%p\n", skb->head);
}

int main() {
    struct sk_buff skb;
    skb_over_panic(&skb);
    return 0;
}

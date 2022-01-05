// Size test from TinyOS, courtesy of David Gay.

typedef unsigned char uint8_t;
typedef struct { unsigned char data[1]; } nx_uint8_t;

typedef struct CC1KHeader {
    nx_uint8_t length;
} cc1000_header_t;

IFTEST t1: success
uint8_t __nesc_ntoh_uint8(const void *SIZE(1) source) {
    const uint8_t *base = source;
    return base[0];
}
ENDIF

IFTEST t2: success
uint8_t __nesc_ntoh_uint8(nx_uint8_t *source) {
    const uint8_t *base = source->data;
    return base[0];
}
ENDIF

cc1000_header_t foo;

int main(void) {
    cc1000_header_t *txHeader = &foo;
    foo.length.data[0] = 0;
    return (1 < __nesc_ntoh_uint8(&txHeader->length)) ? 1 : 0;
}

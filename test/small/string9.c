// This test makes sure that a non-NT pointer can't access that last zero.

// KEEP baseline: success

void my_strncpy(short * COUNT(4) NT dest, const short * NTS NONNULL src)
{
	short *tmp = dest;
	short *src2 = src; // KEEP baseline
	short *src2 = NTDROP(src); // KEEP error1: error = Assertion

	while ((*tmp = *src2) != '\0') {
	    tmp++;
	    src2++;
	}
}

int main() {
    short (NT buf)[5];
    short (NT src)[5] = { 1,2,3,4,0 };
    my_strncpy(buf, src);
    return 0;
}

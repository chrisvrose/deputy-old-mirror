// This test makes sure that a non-NT pointer can't access that last zero.

// KEEP baseline: success

void my_strcpy(char * TRUSTED dest, const char * NTS src)
{
	char * SEQ tmp = dest;
	char * NTS src2 = src; // KEEP baseline
	char *src2 = NTDROP(src); // KEEP error1: error = Assertion

	while ((*dest = *src2) != '\0') {
	    dest++;
	    src2++;
	}
}

int main() {
    char buf[5];
    my_strcpy(buf, "test");
    return 0;
}

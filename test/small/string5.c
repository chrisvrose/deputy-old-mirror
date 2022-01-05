static char * strncat(char * TRUSTED dest, const char * NTS src, int count)
{
	char * SEQ tmp = dest;

	if (count) {
		while (*dest)
			dest++;
		while ((*dest++ = *src++) != 0) {
			if (--count == 0) {
				*dest = '\0';
				break;
			}
		}
	}

	return tmp;
}

int main() {
	char dest[16] = "a";
	char *src = "bcd";
	strncat(dest, src, 3);
	return 0;
}

static int strncmp(const char * NT cs, const char * NT ct, int count)
{
	register signed char __res = 0;

	while (count) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
		count--;
	}

	return __res;
}

int main() {
	char * s1 = "abcd";
	char * s2 = "abce";
	
	strncmp(s1, s2, 3);
	return 0;
}

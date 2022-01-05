static int strncmp(const char * NTS cs, const char * NTS ct, int count)
{
	int i = 0;
	for (; i < count-1 && cs[i] && ct[i] && cs[i] == ct[i]; i++)
	{}
	return cs[i] - ct[i];
}

int main() {
	char * NTS s1 = "bcd";
	char * NTS s2 = "bcde";
	strncmp(s1, s2, 5);
	return 0;
}

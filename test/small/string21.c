// Tests *s++.

int main() {
    char * NTS s = "test";
    int i = 0;
    while (*s++) {
	i++;
    }
    return (i == 4);
}

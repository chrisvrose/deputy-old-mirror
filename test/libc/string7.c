// String tests from Zach.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

IFTEST str-qs-1 : success
#define NUMSTRINGS 11
char *NTS strings[] = { "abracadabra",
		     "bamboozled",
		     "Canada Dry Ginger Ale",
		     "boston champion",
		     "Shostakovich: Symphony No. 10",
		     "PRINTED LINING FOR PRIVACY",
		     "www.asus.com",
		     "Curiously Strong Peppermints",
		     "o",
		     "\0",
		     "" };

void reverse(char *NTS s)
{
        int c, i, j;

        for(i = 0, j = strlen(s) - 1; i < j; i++, j--) {
                c = s[i];
                s[i] = s[j];
                s[j] = c;
        }
}

//s must have room for '-' and a 10 digit number
char *itoa(int n, char * COUNT(11) s, int w)
{
        int i, sign;

        if ( (sign = n) < 0 ) {
                n = -n;
        }

        i = 0;
        do {
                s[i++] = n % 10 + '0';
        } while ( (n /= 10) > 0 );

        if (sign < 0) {
                s[i++] = '-';
        }

        while(i < w) {
                s[i++] = ' ';
        }

        s[i]= '\0';

        reverse(s);
        return s;
}

int str_compare(const void *v1, const void *v2)
{
	char *s1 = *(char **)v1;
	char *s2 = *(char **)v2;
	int s1l = strlen(s1);
	int s2l = strlen(s2);
	int n1 = atoi(s1 + s1l - 2);
	int n2 = atoi(s2 + s2l - 2);

	if( n1 > n2 )
		return 1;
	if( n2 > n1 )
		return -1;
	return 0;
}

void test()
{
	char *copy[NUMSTRINGS];
	char num[3];
	int i, j, l, sum;

	for( i = 0; i < NUMSTRINGS; i++ ) {
		l = strlen(strings[i]);
		copy[i] = malloc( l + 3 );
		strncpy( copy[i], strings[i], l );

		sum = 0;
		for( j = 0; j < l; j++ ) {
			sum += 11 * copy[i][j] + 7;
		}
		sum %= 97;

		itoa( sum, num, 2 );
		strncat( copy[i], num, 2 );
	}

	qsort( copy, NUMSTRINGS, sizeof(copy[1]), str_compare );

	for( i = 0; i < NUMSTRINGS; i++ ) {

		printf( "%s\n", copy[i] );

	}

	return;
}

int main()
{
	test();
	return 0;
}
ENDIF

TESTDEF str-qsf-1 : error
IFTEST str-qsf-1
#define NUMSTRINGS 11
char * NTS
strings[] = { "abracadabra",
		     "bamboozled",
		     "Canada Dry Ginger Ale",
		     "boston champion",
		     "Shostakovich: Symphony No. 10",
		     "PRINTED LINING FOR PRIVACY",
		     "www.asus.com",
		     "Curiously Strong Peppermints",
		     "o",
		     "\0",
		     "" };

void reverse(char * NTS s)
{
        int c, i, j;

        for(i = 0, j = strlen(s) - 1; i < j; i++, j--) {
                c = s[i];
                s[i] = s[j];
                s[j] = c;
        }
}

char * NTS itoa(int n, char * NT COUNT(w) s, int w)
{
        int i, sign;

        if ( (sign = n) < 0 ) {
                n = -n;
        }

        i = 0;
        do {
                s[i++] = n % 10 + '0';
        } while ( (n /= 10) > 0 );

        if (sign < 0) {
                s[i++] = '-';
        }

        while(i < w) {
                s[i++] = ' ';
        }

        s[i]= '\0';

        reverse(s);
        return s;
}

int str_compare(char * NTS * v1, char * NTS * v2)
{
	char *s1 = *(char * NTS *)v1;
	char *s2 = *(char * NTS *)v2;
	int s1l = strlen(s1);
	int s2l = strlen(s2);
	int n1 = atoi(s1 + s1l - 2);
	int n2 = atoi(s2 + s2l - 2);

	if( n1 > n2 )
		return 1;
	if( n2 > n1 )
		return -1;
	return 0;
}

void test()
{
	char *copy[NUMSTRINGS];
	char num[3];
	int i, j, l, sum;

	for( i = 0; i < NUMSTRINGS; i++ ) {
		l = strlen(strings[i]);
		copy[i] = malloc( l + 3 );
		strncpy( copy[i], strings[i], l );

		sum = 0;
		for( j = 0; j <= l + 3; j++ ) {
			sum += 11 * copy[i][j] + 7;
		}
		sum %= 97;

		itoa( sum, num, 2 );
		strncat( copy[i], num, 2 );
	}

	qsort( copy, NUMSTRINGS, sizeof(copy[1]), str_compare );

	for( i = 0; i < NUMSTRINGS; i++ ) {

		printf( "%s\n", copy[i] );

	}

	return;
}

int main()
{
	test();
	return 0;
}
ENDIF

TESTDEF "str-wl-1" : success
IFTEST "str-wl-1"
#define NUMSTRINGS 9
char *NTS strings[] = { "abracadabra",
		     "bamboozled",
		     "Canada Dry Ginger Ale",
		     "boston champion",
		     "Shostakovich: Symphony No. 10",
		     "PRINTED LINING FOR PRIVACY",
		     "www.asus.com",
		     "Curiously Strong Peppermints",
		     "o"};

void test()
{
	char *copy[NUMSTRINGS];
	int i, l;

	for( i = 0; i < NUMSTRINGS; i++ ) {
		l = strlen( strings[i] );
                //matth: we don't support this
		copy[i] = malloc( l + 2 );
		strncpy( copy[i], strings[i], l );

		copy[i][l] = copy[i][l-1];
		copy[i][l+1] = '\0';
		printf( "%s\n", copy[i] );
	}

	return;
}

int main()
{
	test();
	return 0;
}
ENDIF

TESTDEF "str-wlf-1" : error
IFTEST "str-wlf-1"
#define NUMSTRINGS 11
char * NTS
strings[] = { "abracadabra",
		     "bamboozled",
		     "Canada Dry Ginger Ale",
		     "boston champion",
		     "Shostakovich: Symphony No. 10",
		     "PRINTED LINING FOR PRIVACY",
		     "www.asus.com",
		     "Curiously Strong Peppermints",
		     "o",
		     "\0",
		     "" };

void test()
{
	char * NTS copy[NUMSTRINGS];
	int i, l;

	for( i = 0; i < NUMSTRINGS; i++ ) {
		l = strlen( strings[i] );
		copy[i] = malloc( l + 2 );
		strncpy( copy[i], strings[i], l );

		copy[i][l] = copy[i][l-1];
		copy[i][l+1] = '\0';
		printf( "%s\n", copy[i] );
	}

	return;
}

int main()
{
	test();
	return 0;
}
ENDIF

TESTDEF "str-wsqpp-1" : success
IFTEST "str-wsqpp-1"
#define NUMSTRINGS 11
char * NTS strings[] = { "abracadabra",
		     "bamboozled",
		     "Canada Dry Ginger Ale",
		     "boston champion",
		     "Shostakovich: Symphony No. 10",
		     "PRINTED LINING FOR PRIVACY",
		     "www.asus.com",
		     "Curiously Strong Peppermints",
		     "o",
		     "\0",
		     "" };

void test()
{
	char * NTS copy[NUMSTRINGS];
	char *q;
	int i, j, l, sum;

	for( i = 0; i < NUMSTRINGS; i++ ) {
		l = strlen( strings[i] );
		copy[i] = strdup( strings[i] );
		strncpy( copy[i], strings[i], l );

		sum = 0;
		q = copy[i];
		while( *q++ ) {
			sum += *(q - 1);
			*(q - 1) = 'A';
		}
	}

	return;
}

int main()
{
	test();
	return 0;
}
ENDIF

TESTDEF "str-wsqppf-1" : error
IFTEST "str-wsqppf-1"
#define NUMSTRINGS 11
char *strings[] = { "abracadabra",
		     "bamboozled",
		     "Canada Dry Ginger Ale",
		     "boston champion",
		     "Shostakovich: Symphony No. 10",
		     "PRINTED LINING FOR PRIVACY",
		     "www.asus.com",
		     "Curiously Strong Peppermints",
		     "o",
		     "\0",
		     "" };

void test()
{
	char *copy[NUMSTRINGS];
	char *q;
	int i, j, l, sum;

	for( i = 0; i < NUMSTRINGS; i++ ) {
		l = strlen( strings[i] );
		copy[i] = strdup( strings[i] );
		strncpy( copy[i], strings[i], l );

		sum = 0;
		q = copy[i];
		while( *q++ ) {
			sum += *(q);
			*(q) = 'A';
		}
	}

	return;
}

int main()
{
	test();
	return 0;
}
ENDIF

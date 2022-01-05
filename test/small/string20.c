// String tests from Jeremy.

#include "harness.h"

int my_strlen(char* NTS s) {
  int n = 0;
  while (*s != 0) {
    n++;
    s++;
  }
  return n;
}

void my_strlcpy(char* COUNT(n-1) NT dest, char* NTS src, int n) {
  int position;
  for (position = 0; position < n-1 && *src != 0; src++, position++) {
      dest[position] = *src;
  }
  if (position < n) {
      *dest = 0;
  }
}

void my_strlcat(char* COUNT(n-1) NT dest, char* NTS src, int n) {
  int offset = my_strlen(dest);
  my_strlcpy(dest + offset, src, n - offset);
}

char* NTS my_strdup(char* NTS s1) {
  int i;
  int len = my_strlen(s1);
  char * s2 = malloc(len+1);
  for (i = 0; i < len; i++) {
    s2[i] = s1[i];
  }
  s2[len] = 0;
  return s2;
}


TESTDEF str-iter-1 : success
IFTEST str-iter-1
void test(char * NTS str) {
    while (*str != 0) {
        str++;
    }
}
int main() {
    test("test string");
    return 0;
}
ENDIF

TESTDEF str-iter-2even : success
IFTEST str-iter-2even
void test(char * NTS str) {
    while (*str != 0) {
        str += 2;
    }
}
int main() {
  test("test string."); //buffer length = 13, so [0]...[12] are accessible.
  return 0;
}
ENDIF

TESTDEF str-iter-stride4 : error
IFTEST str-iter-stride4
void test(char * NTS str) {
    while (*str != 0) {
        str += 4;
    }
}
int main() {
    test("test strng");
    return 0;
}
ENDIF

TESTDEF str-iter-3 : success
IFTEST str-iter-3
void test(char * NTS str) {
    int i;
    int sum = 0;
    for (i = 0; str[i] != 0; i++) {
        sum += str[i];
    }
}
int main() {
    test("test string");
    return 0;
}
ENDIF

IFTEST str-iter-4 : error
void test(char * NTS str) {
    int i;
    int sum = 0;
    for (i = 0; str[i] != 0; i += 4) {
        sum += str[i];
    }
}
int main() {
    test("test strng");
    return 0;
}
ENDIF

TESTDEF str-iter-5 : success
IFTEST str-iter-5
void test(char * NTS str) {
    int sum = 0;

    while (*str != 0) {
        while (*str == ' ') {
            str++;
        }

        while (*str != 0 && *str != ' ') {
            sum += *str;
            str++;
        }
    }
}
int main() {
    test("test string");
    return 0;
}
ENDIF

TESTDEF str-iter-6 : error
IFTEST str-iter-6
void test(char * NTS str) {
    int sum = 0;

    while (*str != 0) {
        while (*str == ' ') {
            str++;
        }

        while (*str != ' ') {
            sum += *str;
            str++;
        }
    }
}
int main() {
    test("test string");
    return 0;
}
ENDIF

TESTDEF my_strlen-1 : success
IFTEST my_strlen-1
void test(char * NTS str) {
    int i;
    int sum = 0;
    int n = my_strlen(str);
    for (i = 0; i < n; i++) {
        sum += str[i];
    }
}
int main() {
    test("test string");
    return 0;
}
ENDIF

TESTDEF my_strlen-2 : success
IFTEST my_strlen-2
void test(char * NTS str) {
    int i;
    int sum = 0;
    int n = my_strlen(str);
    for (i = 0; i < n / 2; i++) {
        sum += str[i];
    }
}
int main() {
    test("test string");
    return 0;
}
ENDIF

TESTDEF strlcpy-1 : success
IFTEST strlcpy-1
void test(char * NTS str) {
    char (NT buf)[5];
    my_strlcpy(buf, str, 5);
}
int main() {
    test("test string");
    return 0;
}
ENDIF

TESTDEF strlcpy-2 : success
IFTEST strlcpy-2
void test(char * NTS str) {
    int len = my_strlen(str);
    char *buf = alloc(char, len + 1);
    my_strlcpy(buf, str, len + 1);
}
int main() {
    test("test string");
    return 0;
}
ENDIF

TESTDEF strlcpy-3 : error
IFTEST strlcpy-3
void test(char * NTS str) {
    int len = my_strlen(str);
    char *buf = alloc(char, len);
    my_strlcpy(buf, str, len + 1);
}
int main() {
    test("test string.");
    return 0;
}
ENDIF

TESTDEF strlcat-1 : success
IFTEST strlcat-1
void test(char * NTS str) {
    char (NT buf)[15];
    my_strlcpy(buf, str, 15);
    my_strlcat(buf, str, 15);
}
int main() {
    test("test string");
    return 0;
}
ENDIF

TESTDEF strlcat-2 : error
IFTEST strlcat-2
void test(char * NTS str) {
    char (NT buf)[15];
    my_strlcpy(buf, str, 16);
    my_strlcat(buf, str, 16);
}
int main() {
    test("test string");
    return 0;
}
ENDIF

TESTDEF struct-1 : success
IFTEST struct-1
typedef struct foo foo;
struct foo {
    char * COUNT(len) buf;
    int len;
};
void test1(foo *p) {
    char *buf = alloc(char, 10);
    p->len = 10;
    p->buf = buf;
}
void test2(foo *p) {
    int i;
    int sum = 0;
    for (i = 0; i < p->len; i++) {
        sum += p->buf[i];
    }
}
int main() {
    foo f;
    test1(&f);
    test2(&f);
    return 0;
}
ENDIF

TESTDEF struct-2 : error
IFTEST struct-2
typedef struct foo foo;
struct foo {
    char * COUNT(len) buf;
    int len;
};
void test1(foo *p) {
    char *buf = alloc(char, 12);
    p->len = 13;
    p->buf = buf;
}
void test2(foo *p) {
    int i;
    int sum = 0;
    for (i = 0; i < p->len; i++) {
        sum += p->buf[i];
    }
}
int main() {
    foo f;
    test1(&f);
    test2(&f);
    return 0;
}
ENDIF

TESTDEF struct-3 : success
IFTEST struct-3
typedef struct foo foo;
struct foo {
  char * NTS str;
};
void test1(foo *p) {
  p->str = my_strdup("test string");
}
void test2(foo *p) {
  int i;
  int sum = 0;
  int len = strlen(p->str);
  for (i = 0; i < len; i++) {
    sum += p->str[i];
  }
}
int main() {
  foo f;
  test1(&f);
  test2(&f);
  return 0;
}
ENDIF

TESTDEF struct-4 : success
IFTEST struct-4
typedef struct foo foo;
struct foo {
    char * NTS str;
};
void test1(foo *p) {
    p->str = alloc(char, 10);
}
void test2(foo *p) {
    int i;
    int sum = 0;
    int len = my_strlen(p->str);
    for (i = 0; i < len; i++) {
        sum += p->str[i];
    }
}
int main() {
    foo f;
    test1(&f);
    test2(&f);
    return 0;
}
ENDIF

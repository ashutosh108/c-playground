#include "lcs.h"
#include "common.h"

#include <stdio.h>
#include <string.h>

int
cmp_char(const void *a, const void *b)
{
	const char c1 = *(const char *)a;
	const char c2 = *(const char *)b;
	if (c1 < c2)
		return -1;
	else if (c1 == c2)
		return 0;
	else
		return +1;
}

void
test_lcs(const char *a, const char *b, const char *expected_lcs)
{
	int res_len;
	struct timespec start_time = timespec_now();
//	char *actual_lcs = lcs_str(a, b);
	char *actual_lcs = lcs(a, strlen(a), b, strlen(b), 1, cmp_char, &res_len);
	bool ok = (strcmp(expected_lcs, actual_lcs) == 0);
	printok(ok, &start_time, "lcs('%s', '%s')", a, b);
	if (!ok) {
		printf("expected: '%s'\n", expected_lcs);
		printf("actual  : '%.*s'\n", res_len, actual_lcs);
	}
	free(actual_lcs);
}

int main() {
	const char *s1 = "abc";
	const char *s2 = "acd";
	int res_len;
	const char *res = lcs(s1, strlen(s1), s2, strlen(s2), sizeof *s1, cmp_char, &res_len);
	printf("res=(%d)'%.*s'\n", res_len, res_len, res);
	test_lcs("", "", "");
	test_lcs("a", "a", "a");
	test_lcs("a", "b", "");
	test_lcs("a", "ab", "a");
	test_lcs("abc", "bc", "bc");
	test_lcs("qwe", "ABCqweDEF", "qwe");
	test_lcs("ABCqweDEF", "qwe", "qwe");
	test_lcs("abcdef", "bQcdef", "bcdef");
}

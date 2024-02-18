#include "lcs.h"
#include "common.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void *
lcs(const void *a, int a_len, const void *b, int b_len, int el_size, Cmp cmp, int *res_len)
{
	int **max_len;
	char *res;

#if DEBUG
	for (int i=0; i < a_len; i++) {
		const char *s = ((char **)a)[i];
		printf("lcs a %d '%s'\n", i, s);
	}

	for (int i=0; i < b_len; i++) {
		const char *s = ((char **)b)[i];
		printf("lcs a %d '%s'\n", i, s);
	}
#endif

	if (a_len == 0 || b_len == 0) {
		res = malloc(1);
		*res = '\0';
		return res;
	}

	max_len = malloc( (a_len+1) * sizeof *max_len );
	for (int i=0; i<=a_len; i++)
		max_len[i] = calloc(b_len+1, sizeof **max_len);

	for (int i = a_len - 1; i >= 0; i--) {
		for (int j = b_len - 1; j >= 0; j--) {
			const char **a_i = (const char**) ( (char*)a + i * el_size );
			const char **b_j = (const char**) ( (char*)b + j * el_size );
#if DEBUG
			printf("a=%p, a[%d]=%p('%s') b[%d]=%p('%s')\n", a, i, *a_i, *a_i, j, *b_j, *b_j);
#endif
			if (cmp(a_i, b_j) == 0)
				max_len[i][j] = 1 + max_len[i+1][j+1];
			else
				max_len[i][j] = max(max_len[i+1][j], max_len[i][j+1]);
		}
	}
#if DEBUG
	printf("max_len(%d x %d):\n", a_len+1, b_len+1);
	for (int i=0; i <= a_len; i++) {
		for (int j=0; j <= b_len; j++) {
			printf("%d\t", max_len[i][j]);
		}
		printf("\n");
	}
#endif

	*res_len = max_len[0][0];
	res = malloc( 1 + *res_len * el_size);
	for (int i=0, j=0, res_i=0; max_len[i][j] != 0 && i < a_len && j < b_len; ) {
		if (cmp((char*)a + i * el_size, (char*)b + j * el_size) == 0) {
			memcpy(res + res_i * el_size, a + i * el_size, el_size);
#if DEBUG
			const char *s;
			memcpy(&s, res + res_i * el_size, el_size);
			printf("res[%d]='%s'\n", res_i, s);
#endif
			res_i++;
			i++;
			j++;
		} else {
			if (max_len[i][j] == max_len[i+1][j])
				i++;
			else
				j++;
		}
	}
	res[*res_len * el_size] = '\0';

	for (int i=0; i <= a_len; i++)
		free(max_len[i]);
	free(max_len);
	return res;
}

static inline
int cmp_char(const void *a, const void *b)
{
	char c1 = *(char*)a;
	char c2 = *(char*)b;
	if (c1 < c2)
		return -1;
	else if (c1 == c2)
		return 0;
	else
		return +1;
}

char *
lcs_str(const char *a, const char *b)
{
	int ret_len;
	return lcs(a, strlen(a), b, strlen(b), 1, cmp_char, &ret_len);
}

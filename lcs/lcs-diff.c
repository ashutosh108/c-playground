#include "common.h"
#include "lcs.h"

#include <string.h>

inline static int cmp_str(const void *a, const void *b)
{
	const char * const *s1 = a;
	const char * const *s2 = b;
	//printf("cmp('%s', '%s') == %d\n", *s1, *s2, strcmp(*s1,*s2));
	return strcmp(*s1, *s2);
}

char **slurp_lines(const char *filename, int *line_count)
{
	int res_capacity = 16;
	char **res = malloc(res_capacity * sizeof *res);
	int line_no;

	FILE *f = fopen(filename, "r");
	if (f == NULL) die(filename);
	for (line_no = 0; line_no < res_capacity; line_no++) {
		size_t line_size = 0;
		res[line_no] = NULL;
		if (getline(&res[line_no], &line_size, f) == -1) {
			if (ferror(f))
				die("reading from %s", filename);
			break;
		}
		if (line_no+1 == res_capacity) {
			res = realloc(res, res_capacity * sizeof *res);
		}
	}
	fclose(f);
	if (line_count)
		*line_count = line_no;
	return res;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		die("usage: lcs-diff file1 file2");
	}
	int l1_size, l2_size;
	char **l1 = slurp_lines(argv[1], &l1_size);
	char **l2 = slurp_lines(argv[2], &l2_size);
	int lcs_size;

	char **common_lines = lcs(l1, l1_size, l2, l2_size, sizeof *l1, cmp_str, &lcs_size);
	printf("%d %d - %d common lines\n", l1_size, l2_size, lcs_size);
	for (int line_l1=0, line_l2=0, line_lcs=0; line_l1 < l1_size || line_l2 < l2_size || line_lcs < lcs_size; ) {
		bool lcs_valid = line_lcs < lcs_size;
		bool line1_valid = line_l1 < l1_size;
		bool line2_valid = line_l2 < l2_size;
		const char *linec = lcs_valid  ? common_lines[line_lcs] : NULL;
		const char *line1 = line1_valid ? l1[line_l1] : NULL;
		const char *line2 = line2_valid ? l2[line_l2] : NULL;
		bool		eq1 = line1_valid && lcs_valid && strcmp(linec, line1) == 0;
		bool		eq2 = line2_valid && lcs_valid && strcmp(linec, line2) == 0;
		if (eq1 && eq2) {
			printf(" %s", linec);
			line_l1++;
			line_l2++;
			line_lcs++;
		} else if (eq2 && line_l1 < l1_size) {
			printf("-%s", line1);
			line_l1++;
		} else if (eq1 && line_l2 < l2_size) {
			printf("+%s", line2);
			line_l2++;
		} else if (line_l1 < l1_size) {
			printf("-%s", line1);
			line_l1++;
		} else if (line_l2 < l2_size) {
			printf("+%s", line2);
			line_l2++;
		} else {
			die1("unexpected");
		}
	}
	// for (int i=0; i < lcs_size; i++) {
	// 	printf("%-4d %s", i, common_lines[i]);
	// }
}
